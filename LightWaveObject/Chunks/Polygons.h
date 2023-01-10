#pragma once
#include "Chunk.h"
#include "ChunkDefinitions.h"

class Polygons : public Chunk {
public:

	// Constructor
	Polygons() : Chunk(ChunkTag::POLS) { }

	// Public methods
	string getDescription();
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

	// Getters
	vector<POLYGON>& getPolygons();

private:

	// Private methods
	vector<POLYGON> parsePolygons(char rawBuffer[], unsigned chunkLength);

	// Private data
	vector<POLYGON> polygons_;
};

