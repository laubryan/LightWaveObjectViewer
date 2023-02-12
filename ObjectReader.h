#pragma once

#include <DirectXMath.h>
#include <filesystem>
#include <windows.h>

#include "LightWaveObject/LightWaveObject.h"
#include "LightWaveObject/Chunks/Surface.h"
#include "RendererDefinitions.h"

class ObjectReader {
public:

	// Getters
	std::vector<WORD>	GetIndices();
	std::vector<VERTEX> GetVertices();
	int GetNumLayers();
	int GetNumNonTriangles();
	int	GetNumTriangles();

	// Public methods
	bool ReadObjectFile(std::string objectPathname, std::string& errorReason);

private:

	// Private member functions
	bool TransferMeshDataFromLWO(unique_ptr<LightWaveObject> obj, std::string& errorReason);

	// Private data
	bool _objectLoaded {};

	// Mesh
	std::vector<VERTEX> _vertices;
	std::vector<WORD> _indices;
	int _numLayers;
	int _numTriangles;
	int _numNonTriangles;
};

