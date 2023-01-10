#pragma once
#include "Chunk.h"
class Description : public Chunk {
public:

	// Constructor
	Description() : Chunk(ChunkTag::DESC) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data

};

