#pragma once
#include <memory>

#include "ChunkDefinitions.h"

class Chunk {
public:
	// Constructor
	Chunk(ChunkTag tag = ChunkTag::UNKNOWN) : _tag { tag } { }

	// Static factory methods
	static unique_ptr<Chunk> create(ChunkTag chunkType);
	
	// Public methods
	ChunkTag getTag();
	unique_ptr<char[]> read(char rawBuffer[], LWO_CHUNK_HEADER header);

	// Virtual methods
	virtual string getDescription();
	virtual void parse(char rawBuffer[], LWO_CHUNK_HEADER header);

private:

	// Private data
	ChunkTag _tag {};
};

