#include "ObjectReader.h"


/// <summary>
/// Read and parse the object file
/// </summary>
/// <param name="objectPathname">Full path and filename for the object file</param>
/// <param name="errorReason">Reason for the failure</param>
/// <returns>Read success</returns>
bool ObjectReader::ReadObjectFile(string objectPathname, wstring& errorReason) {

	// Initialize state
	_objectLoaded = false;
	_vertices.clear();
	_indices.clear();

	// Verify that the file exists
	if (!std::filesystem::exists(objectPathname)) {
		errorReason = L"The file does not exist";
		return false;
	}

	// Read designated object file
	std::unique_ptr<LightWaveObject> lwObject = make_unique<LightWaveObject>();
	if (!lwObject->Read(objectPathname, errorReason)) {

		// Assign generic error if none returned
		if (errorReason == L"") {
			errorReason = L"The file could not be read";
		}

		return false;
	}

	// Transfer mesh data
	errorReason = L"";
	if (!TransferMeshDataFromLWO(move(lwObject), errorReason)) {
		if (errorReason == L"") {
			errorReason = L"Could not transfer mesh data from object file";
		}
		return false;
	}

	// Set successful load flag
	_objectLoaded = true;

	return true;
}

/// <summary>
/// Get a copy of the object's indices
/// </summary>
/// <returns>Vector of object indices</returns>
std::vector<WORD> ObjectReader::GetIndices() {
	return _indices;
}

/// <summary>
/// Get number of layers
/// </summary>
/// <returns>Number of layers</returns>
int ObjectReader::GetNumLayers() {
	return _numLayers;
}

/// <summary>
/// Get number of non triangles retrieved
/// </summary>
/// <returns>Number of non triangles</returns>
int ObjectReader::GetNumNonTriangles() {
	return _numNonTriangles;
}

/// <summary>
/// Get number of triangles retrieved
/// </summary>
/// <returns>Number of triangles</returns>
int ObjectReader::GetNumTriangles() {
	return _numTriangles;
}

/// <summary>
/// Get a copy of the object's vertices
/// </summary>
/// <returns>Vector of object vertices</returns>
std::vector<VERTEX> ObjectReader::GetVertices() {
	return _vertices;
}

/// <summary>
/// Transfer mesh data from LightWave object to renderer
/// </summary>
/// <param name="obj">LightWave object</param>
/// <returns>Transfer success</returns>
bool ObjectReader::TransferMeshDataFromLWO(unique_ptr<LightWaveObject> obj, wstring& errorReason) {

	// Record some data on the loaded object
	_numTriangles = 0;					// Number of triangles extracted
	_numNonTriangles = 0;				// Polygons with unsupported number of vertices
	_numLayers = obj->GetNumLayers();	// Number of LightWave object layers

	// Validate object layers
	if (_numLayers == 0) return false;

	// Get surface for this layer
	Surface::COLOR col;
	Surface* surface = obj->GetSurfaceByLayer(0);
	if (surface) {
		// If a surface exists, then extract its color
		col = surface->getColor();
	}
	else {
		// Otherwise assign the default color
		col = Surface::COLOR { 1.0f, 1.0f, 1.0f };
	}

	// Initialize vertex color
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(col.r, col.g, col.b, 1.0f);

	// Transfer LightWave vertices to temporary list
	vector<VERTEX> lwVertices;
	const vector<VEC12>& points = obj->GetPointsByLayer(0);
	for (auto& point : points) {
		VERTEX vertex = { DirectX::XMFLOAT3(point.X, point.Y, point.Z) };
		lwVertices.push_back(vertex);
	}

	// Transfer polygon indices
	const vector<POLYGON>& pols = obj->GetPolsByLayer(0);
	unsigned targetIndexOffset = 0;
	for (auto pol : pols) {

		// Note that LightWave polygons have CW winding order
		// so the vertex order must be reversed to CCW
		if (pol.numVertices > 2) {

			// Set up initial triangle source indices
			unsigned sourceIndexOffset = 0;
			unsigned sourceIndex1 = pol.pointIndex[sourceIndexOffset + 0];
			unsigned sourceIndex2 = pol.pointIndex[sourceIndexOffset + 1];
			unsigned sourceIndex3 = pol.pointIndex[sourceIndexOffset + 2];
			sourceIndexOffset += 3;

			// Get initial source triangle vertices
			VERTEX vert1 = lwVertices[sourceIndex1];
			VERTEX vert2 = lwVertices[sourceIndex2];
			VERTEX vert3 = lwVertices[sourceIndex3];

			// Calculate vertex normal
			// This should be the same for all vertices of this polygon/face
			DirectX::XMVECTOR vec1 = {
				vert1.pos.x - vert2.pos.x,
				vert1.pos.y - vert2.pos.y,
				vert1.pos.z - vert2.pos.z,
			};
			DirectX::XMVECTOR vec2 = {
				vert1.pos.x - vert3.pos.x,
				vert1.pos.y - vert3.pos.y,
				vert1.pos.z - vert3.pos.z,
			};
			DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vec1, vec2));

			// Store initial triangle vertices
			DirectX::XMStoreFloat3(&vert1.normal, normal);
			DirectX::XMStoreFloat3(&vert2.normal, normal);
			DirectX::XMStoreFloat3(&vert3.normal, normal);
			vert1.color = color;
			vert2.color = color;
			vert3.color = color;
			_vertices.push_back(vert1);
			_vertices.push_back(vert2);
			_vertices.push_back(vert3);

			// Store initial triangle indices
			_indices.push_back(targetIndexOffset + 2); // 2
			_indices.push_back(targetIndexOffset + 1); // 1
			_indices.push_back(targetIndexOffset + 0); // 0

			// Set up for next triangle
			unsigned endTargetIndex = targetIndexOffset + 0;
			unsigned midTargetIndex = targetIndexOffset + 2;
			targetIndexOffset += 3;
			_numTriangles++;

			// Select successive opposite vectors to form each triangle
			for (unsigned vertexIndex = 3; vertexIndex < (unsigned)pol.numVertices; vertexIndex++) {

				// Get the new source vertex
				unsigned newVertexIndex = pol.pointIndex[vertexIndex];
				VERTEX newVertex = lwVertices[newVertexIndex];

				// Assign normal to new vertex
				DirectX::XMStoreFloat3(&newVertex.normal, normal);

				// Assign color to new vertex
				newVertex.color = color;

				// Store new vertex and index
				_vertices.push_back(newVertex);
				_indices.push_back(targetIndexOffset);

				// Store previous two vertices
				_indices.push_back(midTargetIndex);
				_indices.push_back(endTargetIndex);

				// Set up for next triangle
				midTargetIndex = targetIndexOffset; // Next mid vertex is this triangle's first vertex
				targetIndexOffset++;
				_numTriangles++;
			}
		}
		else {
			// There were polygons with invalid numbers of vertices
			_numNonTriangles++;
		}
	}

	// Display warning about unsupported polygons
	if (_numNonTriangles > 0) {
		errorReason = L"Some polygons had an unsupported number of vertices and were skipped.";
	}

	return true;
}
