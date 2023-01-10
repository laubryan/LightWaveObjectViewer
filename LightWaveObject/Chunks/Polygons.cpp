#include "Polygons.h"

/// <summary>
/// Get chunk description
/// </summary>
/// <returns>Description</returns>
string Polygons::getDescription() {
	return "Polygons: " + to_string(_polygons.size());
}

/// <summary>
/// Parse the raw chunk data
/// </summary>
void Polygons::parse(char rawBuffer[], LWO_CHUNK_HEADER header) {

	// Parse by polygon type
	unsigned offset = LWO_CHUNK_DATA_OFFSET;
	string polygonType = CONVERT_BYTES_TO_STRING(rawBuffer + offset, 4);
	if (polygonType == "FACE") {
		_polygons = parsePolygons(rawBuffer + offset, header.length);
	}
}

/// <summary>
/// Get polygons vector
/// </summary>
/// <returns>Vector of polygons</returns>
vector<POLYGON>& Polygons::getPolygons() {
	return _polygons;
}

/// <summary>
/// Parse polygons from raw data
/// </summary>
/// <param name="rawBuffer">Raw buffer</param>
/// <returns>Vector of polygons</returns>
vector<POLYGON> Polygons::parsePolygons(char rawBuffer[], unsigned chunkLength) {

	vector<POLYGON> polygons;

	// Parse all polygons
	unsigned offset = 4;
	while (offset < chunkLength) {

		POLYGON polygon;

		// Get number of vertices in this polygon (could be n-sided)
		unsigned numVertFlags = CONVERT_U2_BYTES_TO_INT((rawBuffer + offset));
		unsigned numVerts = numVertFlags & 0x00ff;
		unsigned flags = (numVertFlags & 0xff00) >> 8;
		offset += 2;

		// Read vertices
		polygon.numVertices = numVerts;
		for (unsigned vertIndex = 0; vertIndex < numVerts; vertIndex++) {

			// Get the index into the PNTS chunk
			unsigned pointIndex = CONVERT_U2_BYTES_TO_INT((rawBuffer + offset));
			polygon.pointIndex.push_back(pointIndex);
			offset += 2;
		}

		// Store polygon
		polygons.push_back(polygon);
	}

	return polygons;
}
