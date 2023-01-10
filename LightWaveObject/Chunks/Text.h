#pragma once
#include "Chunk.h"
class Text : public Chunk {
public:

	// Constructor
	Text() : Chunk(ChunkTag::TEXT) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data

};

