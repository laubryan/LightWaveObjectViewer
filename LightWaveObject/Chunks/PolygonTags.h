#pragma once
#include "Chunk.h"
class PolygonTags : public Chunk {
public:

	// Constructor
	PolygonTags() : Chunk(ChunkTag::PTAG) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data

};

