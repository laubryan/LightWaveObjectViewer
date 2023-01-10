#include "Layer.h"

/// <summary>
/// Add a new chunk to the layer
/// </summary>
/// <param name="chunk"></param>
void Layer::addChunk(unique_ptr<Chunk> chunk) {

	_chunks.push_back(move(chunk));
}

/// <summary>
/// Return chunk at specified index
/// </summary>
/// <param name="chunk">Matching chunk if found</param>
/// <param name="chunkIndex">Index of chunk to return</param>
/// <returns>True if a match was found</returns>
bool Layer::getChunk(Chunk& chunk, unsigned chunkIndex) {

	// Validate index
	if (chunkIndex >= _chunks.size()) {
		return false;
	}

	// Return chunk reference
	chunk = *_chunks[chunkIndex].get();
	return true;
}

/// <summary>
/// Return first chunk matching tag
/// </summary>
/// <param name="chunk">Matching chunk if found</param>
/// <param name="tag">Chunk tag to find</param>
/// <returns>True if a match was found</returns>
Chunk* Layer::getChunk(ChunkTag tag) {

	// Search for the matching tag
	for (auto&& prospective : _chunks) {

		// Return first match
		ChunkTag prospectiveTag = prospective->getTag();
		if (prospectiveTag == tag) {

			// Return a pointer to the internal Chunk
			return prospective.get();
		}
	}

	// No matching chunk found
	return nullptr;
}

/// <summary>
/// Get layer name
/// </summary>
/// <returns>Layer name</returns>
string Layer::getName() {
	return _name;
}

/// <summary>
/// Parse the raw chunk data
/// </summary>
void Layer::parse(char rawBuffer[], LWO_CHUNK_HEADER header) {

	LWO_CHUNK_LAYER_RAW rawChunk;
	LWO_CHUNK_LAYER cookedChunk;

	// Copy non-variable parts of chunk
	memcpy(&rawChunk, rawBuffer, sizeof(LWO_CHUNK_LAYER_RAW));

	// Parse fields
	cookedChunk.tag = LWUtils::convertTagStringTagToEnum(CONVERT_BYTES_TO_STRING(rawChunk.tag));
	cookedChunk.length = CONVERT_U4_BYTES_TO_INT(rawChunk.length);
	cookedChunk.number = CONVERT_U2_BYTES_TO_INT(rawChunk.number);
	cookedChunk.flags = CONVERT_U2_BYTES_TO_INT(rawChunk.flags);
	cookedChunk.pivot = CONVERT_VEC12_BYTES(rawChunk.pivot);
	cookedChunk.name = CONVERT_BYTES_TO_STRING(rawChunk.name);
	
	// Parent field is optional
	unsigned adjustedStringLen = CONVERT_STRING_LENGTH(cookedChunk.name.length());
	size_t offsetToParent = sizeof(LWO_CHUNK_LAYER_RAW) - 1 + adjustedStringLen;
	if (offsetToParent < cookedChunk.length + LWO_CHUNK_DATA_OFFSET) {
		cookedChunk.parent = CONVERT_U2_BYTES_TO_INT((rawBuffer + offsetToParent));
	}
	else {
		cookedChunk.parent = -1;
	}

	// Save some fields to instance
	_name = cookedChunk.name;
}

/// <summary>
/// Get number of chunks in layer
/// </summary>
/// <returns>Number of chunks stored in this layer</returns>
size_t Layer::size() {
	return _chunks.size();
}
