//
// LightWaveObject class
//
// Reads and parses a LightWaveObject file. Supports the following formats:
// 
// - LWO2
// 
#include <filesystem>

#include "LightWaveObject.h"

/// <summary>
/// Read and parse a LightWave object
/// </summary>
/// <param name="lwObjectFilename">Object filename to read</param>
/// <returns>Result code of read operation</returns>
int LightWaveObject::Read(string lwObjectFilename) {

	unique_ptr<char[]> fileBuffer;	// Memory buffer

	// Read the file into memory
	fileBuffer = read(lwObjectFilename);
	if (fileBuffer == nullptr) {

		// Couldn't read the file
		cerr << "Couldn't read the file" << endl;
		return EXIT_FAILURE;
	}

	// Read file header
	LWO_FILE_HEADER fileHeader = parseFileHeader(fileBuffer.get());

	// Not a valid LightWave object
	if (fileHeader.form != "FORM") {
		cerr << "File is not a valid LightWave object file" << endl;
		return EXIT_FAILURE;
	}

	// Not a supported format
	if (fileHeader.id != "LWO2") {
		cerr << "LightWave object version is not supported" << endl;
		return EXIT_FAILURE;
	}

	// Set up to read remaining chunks in file
	size_t offset = sizeof(LWO_FILE_HEADER_RAW);
	size_t fileLength = fileHeader.fileLength;
	size_t CHUNK_HEADER_SIZE = sizeof(LWO_CHUNK_HEADER);
	vector<unique_ptr<Chunk>> orphanedChunks;	// Temporarily hold chunks with no assigned layer

	// Parse all chunks
	unique_ptr<Layer> currentLayer;
	while (offset <= fileLength) {

		// Get chunk header
		LWO_CHUNK_HEADER chunkHeader = parseChunkHeader(fileBuffer.get() + offset);
		//cout << "Chunk: " << LWUtils::convertTagEnumToString(chunkHeader.tag) << " (Offset " << offset << ")" << endl;

		// If it's a new layer then replace the older layer
		if (chunkHeader.tag == ChunkTag::LAYR) {

			// First, save the old layer if there is one
			if (currentLayer != nullptr) {

				// Save the old layer
				_layers.push_back(move(currentLayer));
			}

			// Instantiate and parse the new layer
			unique_ptr<Layer> newLayer = unique_ptr<Layer>(new Layer {});
			newLayer->parse(fileBuffer.get() + offset, chunkHeader);

			// Set new layer as current layer
			currentLayer = move(newLayer);

			// Add any orphaned chunks to the layer
			for (unique_ptr<Chunk>& orphanChunk : orphanedChunks) {
				currentLayer->addChunk(move(orphanChunk));
			}
		}
		else {

			// Instantiate a new chunk object of the appropriate type
			unique_ptr<Chunk> chunk = Chunk::create(chunkHeader.tag);
			if (chunk != nullptr) {

				// Parse the chunk
				chunk->parse(fileBuffer.get() + offset, chunkHeader);

				// Save chunk to current layer
				if (currentLayer != nullptr) {
					currentLayer->addChunk(move(chunk));
				}
				else {

					// No current layer yet, so save to the temp chunk list
					orphanedChunks.push_back(move(chunk));
				}
			}
		}

		// Calculate next offset
		if (chunkHeader.length % 2 == 0) {
			offset = offset + CHUNK_HEADER_SIZE + chunkHeader.length;
		}
		else {
			offset = offset + CHUNK_HEADER_SIZE + chunkHeader.length + 1;
		}
	}

	// Save the last layer if there was one
	if (currentLayer != nullptr) {
		_layers.push_back(move(currentLayer));
	}

	return EXIT_SUCCESS;
}

/// <summary>
/// Display object statistics for debugging
/// </summary>
void LightWaveObject::displayStatistics() {
	for (unique_ptr<Layer>& layer : _layers) {
		cout << "Layer" << endl;

		// Read each chunk in this layer
		unsigned numChunksInLayer = layer->size();
		for (unsigned chunkIndex = 0; chunkIndex < numChunksInLayer; chunkIndex++) {

			// Get chunk from layer
			Chunk chunk;
			layer->getChunk(chunk, chunkIndex);

			// Chunk Tag
			string tagString = LWUtils::convertTagEnumToString(chunk.getTag());

			// Description
			string desc = chunk.getDescription();
			if (desc != "") desc = " (" + desc + ")";

			cout << " Chunk: " << tagString << desc << endl;;
		}
	}
}

/// <summary>
/// Get the number of parsed layers
/// </summary>
/// <returns>Number of layers</returns>
size_t LightWaveObject::GetNumLayers() {
	return _layers.size();
}

/// <summary>
/// Get list of LightWave points for a layer
/// </summary>
/// <param name="layer">Layer index</param>
/// <returns>List of points</returns>
const vector<VEC12>& LightWaveObject::GetPointsByLayer(int layerIndex) {

	// Get reference to target layer
	Layer& layer = *_layers[layerIndex].get();

	// Get PNTS chunk
	Points* points = (Points*)layer.getChunk(ChunkTag::PNTS);

	return points->getPoints();
}

/// <summary>
/// Get list of LightWave polygons for a layer
/// </summary>
/// <param name="layerIndex">Layer index</param>
/// <returns>List of polygons</returns>
const vector<POLYGON>& LightWaveObject::GetPolsByLayer(int layerIndex) {

	// Get reference to target layer
	Layer& layer = *_layers[layerIndex].get();

	// Get POLS chunk
	Polygons* pols = (Polygons*)layer.getChunk(ChunkTag::POLS);

	return pols->getPolygons();
}

/// <summary>
/// Read an LightWave object file
/// </summary>
/// <param name="lwObjectFilename">Object filename to read</param>
/// <returns>Smart pointer to file contents</returns>
unique_ptr<char[]> LightWaveObject::read(std::string lwObjectFilename) {

	unique_ptr<char[]> fileBuffer = nullptr;	// Memory buffer
	ifstream objectFile;	// File stream

	// Read object file into memory
	try {

		// The file must exist
		if (!filesystem::exists(lwObjectFilename)) {
			cerr << "Could not find the file " << lwObjectFilename << "!" << endl;
			return nullptr;
		}

		// Get the file size
		size_t bufferSize = filesystem::file_size(lwObjectFilename);

		// Open the file for reading
		objectFile.open(lwObjectFilename, ios::binary);

		// Allocate the buffer
		fileBuffer = unique_ptr<char[]>(new char[bufferSize]);
		assert(fileBuffer != nullptr);

		// Read file into memory
		objectFile.read(fileBuffer.get(), bufferSize);

		// Close file
		objectFile.close();
	}
	catch (filesystem::filesystem_error ex) {
		// Couldn't get the file size
		cerr << "EXCEPTION: Couldn't read the file size" << endl;
	}
	catch (ifstream::failure ex) {
		// Couldn't read the file
		cerr << "EXCEPTION: Couldn't read the file contents" << endl;
	}
	catch (...) {
		// Mystery error
		cerr << "EXCEPTION: An unknown error occured attempting to read the file" << endl;
	}

	// Finished
	return fileBuffer;
}

/// <summary>
/// Parse chunk header
/// </summary>
/// <param name="rawBuffer">Raw file buffer</param>
/// <returns>Cooked header</returns>
LWO_CHUNK_HEADER LightWaveObject::parseChunkHeader(char rawBuffer[]) {

	LWO_CHUNK_HEADER_RAW rawHeader;
	LWO_CHUNK_HEADER cookedHeader;

	// Copy raw header
	memcpy(&rawHeader, rawBuffer, sizeof(LWO_CHUNK_HEADER_RAW));

	// Parse fields
	string chunkTag = CONVERT_BYTES_TO_STRING(rawHeader.tag, 4);
	cookedHeader.tag = LWUtils::convertTagStringTagToEnum(chunkTag);
	cookedHeader.length = CONVERT_U4_BYTES_TO_INT(rawHeader.length);

	return cookedHeader;
}

/// <summary>
/// Parse object file header
/// </summary>
/// <param name="rawBuffer">Raw file buffer</param>
/// <returns>Cooked header</returns>
LWO_FILE_HEADER LightWaveObject::parseFileHeader(char rawBuffer[]) {

	LWO_FILE_HEADER_RAW rawHeader;
	LWO_FILE_HEADER cookedHeader;

	// Copy raw header
	memcpy(&rawHeader, rawBuffer, sizeof(LWO_FILE_HEADER_RAW));

	// Parse fields
	cookedHeader.form = CONVERT_BYTES_TO_STRING(rawHeader.form, 4);
	cookedHeader.fileLength = CONVERT_U4_BYTES_TO_INT(rawHeader.fileLength);
	cookedHeader.id = CONVERT_BYTES_TO_STRING(rawHeader.id, 4);

	return cookedHeader;
}
