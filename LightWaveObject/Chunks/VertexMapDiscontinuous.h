#pragma once
#include "Chunk.h"
class VertexMapDiscontinuous : public Chunk {
public:

	// Constructor
	VertexMapDiscontinuous() : Chunk(ChunkTag::VMAD) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data

};

