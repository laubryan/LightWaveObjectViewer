//
// LightWave Object Data Chunk Definitions
//
// Definitions of data chunks within the object file
//
#pragma once
#include <intrin.h>
#include <string>
#include <vector>

using namespace std;

/////////////////////////////////////////////////
// Datatypes

// Color
struct COL12 {
	float r;
	float g;
	float b;
};


/////////////////////////////////////////////////
// Chunk tags

enum class ChunkTag { LAYR, PNTS, VMAP, POLS, TAGS, PTAG, VMAD, VMPA, ENVL, CLIP, SURF, BBOX, DESC, TEXT, ICON, UNKNOWN };


/////////////////////////////////////////////////
// File header

// Raw file header
struct LWO_FILE_HEADER_RAW {
	char form[4]; // FORM
	char fileLength[4];
	char id[4]; // ID
};

// Cooked file header
struct LWO_FILE_HEADER {
	string form; // FORM
	size_t fileLength = 0;
	string id; // ID
};


/////////////////////////////////////////////////
// Chunk header

// Raw chunk header
struct LWO_CHUNK_HEADER_RAW {
	char tag[4]; // Tag
	char length[4];
};

// Cooked chunk header
struct LWO_CHUNK_HEADER {
	ChunkTag tag; // Tag
	size_t length;
};

// Offset into chunk data
const size_t LWO_CHUNK_DATA_OFFSET = 8;


/////////////////////////////////////////////////
// Points

struct VEC12 {
	float X;
	float Y;
	float Z;
};


/////////////////////////////////////////////////
// Layer

// Raw layer chunk
struct LWO_CHUNK_LAYER_RAW {
	char tag[4]; // Tag
	char length[4];
	char number[2];
	char flags[2];
	char pivot[12];
	char name[1]; // Placeholder for start of string
};

// Cooked layer chunk
struct LWO_CHUNK_LAYER {
	ChunkTag tag; // Tag
	size_t length;
	unsigned number;
	unsigned flags; // LSB indicates hidden layer
	VEC12 pivot;
	string name;
	unsigned parent; // -1 for no parent
};


/////////////////////////////////////////////////
// Polygon

struct POLYGON {
	int numVertices = 0; // Number of vertices in the polygon
	vector<int> pointIndex; // Vertex indices into PNTS chunk
};


/////////////////////////////////////////////////
// Surface

// Surface Sub-Chunk tags
enum class SurfaceSubChunkTag { COLR, DIFF, LUMI, SPEC, REFL, TRAN, TRNL, GLOS, BLOK, SHRP, BUMP, SIDE, SMAN, RFOP, RIMG, RSAN, RBLR, RIND, TROP, TIMG, TBLR, CLRH, CLRF, ADTR, GLOW, LINE, ALPH, VCOL, UNKNOWN };


/////////////////////////////////////////////////
// Helper functions and macros

// Convert U2 bytes to int
#define CONVERT_U2_BYTES_TO_INT(twobytes) int( \
            (unsigned char)(twobytes[0]) << 8 | \
            (unsigned char)(twobytes[1]) \
        );

// Convert U4 bytes to int
#define CONVERT_U4_BYTES_TO_INT(quadbytes) int( \
            (unsigned char)(quadbytes[0]) << 24 | \
            (unsigned char)(quadbytes[1]) << 16 | \
            (unsigned char)(quadbytes[2]) << 8 | \
            (unsigned char)(quadbytes[3]) \
        );

// Convert 4 byte floats
inline float CONVERT_FLOAT_BYTES(char bytes[]) {
	float val;
	memcpy(&val, bytes, 4);
	return val;
}

// Strings are zero-terminated but are zero-padded for even length
#define CONVERT_STRING_LENGTH(len) unsigned(len % 2 == 0 ? len : len + 1);

// Variable-length index is either 2 or 4 bytes
#define CONVERT_VX_LENGTH(index) unsigned(index < 0xff00 ? 2 : 4);

// Convert little-endian float bytes to big-endian
inline float CONVERT_LE_FLOAT(char* quadBytes) {

	// Set up char pointers to each float
	float srcFloat;
	float dstFloat;
	char* srcChars = (char*)&srcFloat;
	char* dstChars = (char*)&dstFloat;

	// Copy quadbytes from source
	memcpy(&srcFloat, quadBytes, 4);

	// Swap byte orders
	dstChars[0] = srcChars[3];
	dstChars[1] = srcChars[2];
	dstChars[2] = srcChars[1];
	dstChars[3] = srcChars[0];

	return dstFloat;
};

// Convet COL12 bytes to COL12
inline COL12 CONVERT_COL12_BYTES(char col12bytes[]) {
	COL12 col;
	col.r = CONVERT_LE_FLOAT(col12bytes);
	col.g = CONVERT_LE_FLOAT(col12bytes + 4);
	col.b = CONVERT_LE_FLOAT(col12bytes + 8);
	return col;
};

// Convet VEC12 bytes to VEC12
inline VEC12 CONVERT_VEC12_BYTES(char vec12bytes[]) {
	VEC12 vec;
	vec.X = CONVERT_LE_FLOAT(vec12bytes);
	vec.Y = CONVERT_LE_FLOAT(vec12bytes + 4);
	vec.Z = CONVERT_LE_FLOAT(vec12bytes + 8);
	return vec;
};

// Convert fixed array of bytes to a string
inline string CONVERT_BYTES_TO_STRING(char rawString[], unsigned fixedSize = 0) {
	int len = 0;
	if (fixedSize == 0) {
		while (rawString[len] != 0) len++;
		len++;
	}
	else {
		len = fixedSize;
	}
	return string(rawString, len);
};
