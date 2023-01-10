#pragma once
#include <vector>

#include "Chunk.h"

#include "../LWUtils.h"

class Layer : public Chunk {
public:

	// Constructor
	Layer() : Chunk(ChunkTag::LAYR) { }

	// Public methods
	void addChunk(unique_ptr<Chunk> chunk);
	bool getChunk(Chunk& chunk, unsigned chunkIndex);
	Chunk* getChunk(ChunkTag tag);
	string getName();
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;
	size_t size();

private:

	// Private data
	vector<unique_ptr<Chunk>> chunks_;
	string name_;
};

