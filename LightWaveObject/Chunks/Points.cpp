#include "Points.h"

#include <iostream> // Debug

/// <summary>
/// Get chunk description
/// </summary>
/// <returns>Description</returns>
string Points::getDescription() {
	return "Vertices: " + to_string(points_.size());
}

/// <summary>
/// Get points vector
/// </summary>
/// <returns>Vector of points</returns>
vector<VEC12>& Points::getPoints() {
	return points_;
}

/// <summary>
/// Get number of vertices
/// </summary>
/// <returns></returns>
unsigned Points::length() {
	return points_.size();
}

/// <summary>
/// Parse the raw chunk data
/// </summary>
void Points::parse(char rawBuffer[], LWO_CHUNK_HEADER header) {

	// Read all points starting after chunk header
	unsigned stride = sizeof(VEC12);
	for (unsigned offset = LWO_CHUNK_DATA_OFFSET; offset < header.length + LWO_CHUNK_DATA_OFFSET; offset += stride) {

		// Read and parse coordinate
		VEC12 point = CONVERT_VEC12_BYTES(rawBuffer + offset);

		// Store coordinate
		points_.push_back(point);
	}
}

/// <summary>
/// Get number of vertices
/// </summary>
/// <returns>Number of vertices in this chunk</returns>
size_t Points::size() {
	return points_.size();
}
