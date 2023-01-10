#pragma once
#include <vector>

#include "Chunk.h"

class Tags : public Chunk {
public:

	// Constructor
	Tags() : Chunk(ChunkTag::TAGS) { }

	// Public methods
	string getDescription();
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private data
	vector<string> tags_;
};

