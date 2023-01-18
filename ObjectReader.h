#pragma once

#include <DirectXMath.h>
#include <filesystem>
#include <windows.h>

#include "LightWaveObject/LightWaveObject.h"
#include "RendererDefinitions.h"

class ObjectReader {
public:

	// Public methods
	bool ReadObjectFile(std::string objectPathname, std::string& errorReason);
	std::vector<WORD> GetIndices();
	std::vector<VERTEX> GetVertices();

private:

	// Private member functions
	bool TransferMeshDataFromLWO(unique_ptr<LightWaveObject> obj, std::string& errorReason);

	// Private data
	bool _objectLoaded {};

	// Mesh
	std::vector<VERTEX> _vertices;
	std::vector<WORD> _indices;
};

