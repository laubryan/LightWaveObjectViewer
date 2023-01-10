#pragma once
#include <vector>
#include "Chunk.h"
#include "../LightWaveObject.h"

class Points : public Chunk {
public:

	// Constructor
	Points() : Chunk(ChunkTag::PNTS) { }

	// Public methods
	string getDescription() override;
	vector<VEC12>& getPoints();
	unsigned length();
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;
	size_t size();

private:

	// Private data
	vector<VEC12> _points;
};

