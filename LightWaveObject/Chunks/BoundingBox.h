#pragma once
#include "Chunk.h"
class BoundingBox : public Chunk {
public:

	// Constructor
	BoundingBox() : Chunk(ChunkTag::BBOX) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data
};

