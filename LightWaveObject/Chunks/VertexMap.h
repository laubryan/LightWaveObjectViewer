#pragma once
#include "Chunk.h"
class VertexMap : public Chunk {
public:

	// Constructor
	VertexMap() : Chunk(ChunkTag::VMAP) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data

};

