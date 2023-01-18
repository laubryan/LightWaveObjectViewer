#include "ObjectReader.h"


/// <summary>
/// Read and parse the object file
/// </summary>
/// <param name="objectPathname">Full path and filename for the object file</param>
/// <param name="errorReason">Reason for the failure</param>
/// <returns>Read success</returns>
bool ObjectReader::ReadObjectFile(string objectPathname, string& errorReason) {

	// Initialize state
	_objectLoaded = false;
	_vertices.clear();
	_indices.clear();

	// Verify that the file exists
	if (!std::filesystem::exists(objectPathname)) {
		errorReason = "The file does not exist";
		return false;
	}

	// Read designated object file
	std::unique_ptr<LightWaveObject> lwObject = make_unique<LightWaveObject>();
	if (!lwObject->Read(objectPathname, errorReason)) {
		errorReason = "The file could not be read";
		return false;
	}

	// Transfer mesh data
	errorReason = "";
	if (!TransferMeshDataFromLWO(move(lwObject), errorReason)) {
		if (errorReason == "") {
			errorReason = "Could not transfer mesh data from object file";
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
bool ObjectReader::TransferMeshDataFromLWO(unique_ptr<LightWaveObject> obj, string& errorReason) {

	// Count polygons with unsupported number of vertices
	unsigned unsupportedPolygons = 0;

	// Validate object layers
	size_t numLayers = obj->GetNumLayers();
	if (numLayers == 0) return false;

	// Initialize default vertex color
	// TODO: Integrate color from SURF chunk
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

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
			}
		}
		else {
			// There were polygons with invalid numbers of vertices
			unsupportedPolygons++;
		}
	}

	// Display warning about unsupported polygons
	if (unsupportedPolygons > 0) {
		errorReason = std::to_string(unsupportedPolygons) + " polygons had an unsupported number of vertices and were skipped.";
	}

	return true;
}
