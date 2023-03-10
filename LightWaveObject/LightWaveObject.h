#pragma once
#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "LWUtils.h"
#include "Chunks/ChunkDefinitions.h"
#include "Chunks/Chunk.h"
#include "Chunks/Layer.h"
#include "Chunks/Points.h"
#include "Chunks/Polygons.h"
#include "Chunks/Surface.h"

class LightWaveObject {

public:

	// Public methods
	bool Read(std::string lwObjectFilename, wstring& errorReason);
	void displayStatistics();

	// Getters
	size_t GetNumLayers();
	const vector<VEC12>& GetPointsByLayer(int layerIndex);
	const vector<POLYGON>& GetPolsByLayer(int layerIndex);
	Surface* GetSurfaceByLayer(int layerIndex);

private:
	// Private methods
	std::unique_ptr<char[]> readFile(std::string lwObjectFilename);

	LWO_CHUNK_HEADER parseChunkHeader(char rawBuffer[]);
	LWO_FILE_HEADER parseFileHeader(char rawBuffer[]);

	// Object layers
	std::vector<std::unique_ptr<Layer>> _layers;
};
