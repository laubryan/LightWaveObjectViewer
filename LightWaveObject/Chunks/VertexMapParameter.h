#pragma once
#include "Chunk.h"
class VertexMapParameter : public Chunk {
public:

	// Constructor
	VertexMapParameter() : Chunk(ChunkTag::VMPA) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data

};

