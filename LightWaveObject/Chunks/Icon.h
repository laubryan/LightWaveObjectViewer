#pragma once
#include "Chunk.h"
class Icon : public Chunk {
public:

	// Constructor
	Icon() : Chunk(ChunkTag::ICON) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data

};

