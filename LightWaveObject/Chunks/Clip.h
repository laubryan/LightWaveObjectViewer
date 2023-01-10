#pragma once
#include "Chunk.h"
class Clip : public Chunk {
public:

	// Constructor
	Clip() : Chunk(ChunkTag::CLIP) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data

};

