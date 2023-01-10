#include "Chunk.h"

#include "BoundingBox.h"
#include "Clip.h"
#include "Description.h"
#include "Envelope.h"
#include "Icon.h"
#include "Layer.h"
#include "Points.h"
#include "Polygons.h"
#include "PolygonTags.h"
#include "Surface.h"
#include "Tags.h"
#include "Text.h"
#include "VertexMap.h"
#include "VertexMapDiscontinuous.h"
#include "VertexMapParameter.h"

/// <summary>
/// Factory method for Chunks
/// </summary>
/// <param name="chunkType">Chunk type enum</param>
/// <returns>Instantiated chunk object</returns>
unique_ptr<Chunk> Chunk::create(ChunkTag chunkType) {

	// Instantiate chunk
	switch (chunkType) {
		case ChunkTag::LAYR:
			return unique_ptr<Chunk>(new Layer {});
		case ChunkTag::PNTS:
			return unique_ptr<Chunk>(new Points {});
		case ChunkTag::VMAP:
			return unique_ptr<Chunk>(new VertexMap {});
		case ChunkTag::POLS:
			return unique_ptr<Chunk>(new Polygons {});
		case ChunkTag::TAGS:
			return unique_ptr<Chunk>(new Tags {});
		case ChunkTag::PTAG:
			return unique_ptr<Chunk>(new PolygonTags {});
		case ChunkTag::VMAD:
			return unique_ptr<Chunk>(new VertexMapDiscontinuous {});
		case ChunkTag::VMPA:
			return unique_ptr<Chunk>(new VertexMapParameter {});
		case ChunkTag::ENVL:
			return unique_ptr<Chunk>(new Envelope {});
		case ChunkTag::CLIP:
			return unique_ptr<Chunk>(new Clip {});
		case ChunkTag::SURF:
			return unique_ptr<Chunk>(new Surface {});
		case ChunkTag::BBOX:
			return unique_ptr<Chunk>(new BoundingBox {});
		case ChunkTag::DESC:
			return unique_ptr<Chunk>(new Description {});
		case ChunkTag::TEXT:
			return unique_ptr<Chunk>(new Text {});
		case ChunkTag::ICON:
			return unique_ptr<Chunk>(new Icon {});
	}

	// Unhandled chunk
	return nullptr;
}


/// <summary>
/// Get chunk tag
/// </summary>
/// <returns>Tag describing this chunk</returns>
ChunkTag Chunk::getTag() {
	return _tag;
}

/// <summary>
/// Read the raw chunk data
/// </summary>
/// <param name="rawBuffer">Raw file buffer</param>
/// <param name="header">Cooked chunk header</param>
/// <returns>Raw chunk data buffer</returns>
unique_ptr<char[]> Chunk::read(char rawBuffer[], LWO_CHUNK_HEADER header) {

	// Instantiate the chunk buffer
	unique_ptr<char[]> rawChunkBuffer = unique_ptr<char[]>(new char[header.length]);
	
	// Read the raw chunk data from the buffer
	memcpy(rawBuffer, rawChunkBuffer.get(), header.length);

	return rawChunkBuffer;
}

/// <summary>
/// Get descriptive text for debugging
/// </summary>
string Chunk::getDescription() {
	return "";
}

/// <summary>
/// Default parser
/// </summary>
/// <param name="rawBuffer"></param>
/// <param name="header"></param>
void Chunk::parse(char rawBuffer[], LWO_CHUNK_HEADER header) {

	// Override in derived chunk classes
}
