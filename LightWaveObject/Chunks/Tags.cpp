#include "Tags.h"

/// <summary>
/// Get chunk description
/// </summary>
/// <returns>Description</returns>
string Tags::getDescription() {
	string desc = "";
	for (string tag : tags_) {
		desc = desc + tag + ", ";
	}
	return desc;
}

/// <summary>
/// Parse the raw chunk data
/// </summary>
void Tags::parse(char rawBuffer[], LWO_CHUNK_HEADER header) {

	// Extract strings
	size_t offset = LWO_CHUNK_DATA_OFFSET;
	while (offset <= header.length) {

		// Extract and store current string
		string tag = string(rawBuffer + offset);
		tags_.push_back(tag);

		// Seek to next string
		unsigned stringLen = tag.length() + 1;
		offset += stringLen % 2 == 0 ? stringLen : stringLen + 1;
	}
}
