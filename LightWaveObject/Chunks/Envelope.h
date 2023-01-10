#pragma once
#include "Chunk.h"
class Envelope : public Chunk {public:

	// Constructor
	Envelope() : Chunk(ChunkTag::ENVL) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data


};

