#pragma once
#include "Chunks/ChunkDefinitions.h"

class LWUtils {
public:
	static ChunkTag convertTagStringTagToEnum(string tag);
	static string convertTagEnumToString(ChunkTag tagEnum);

	static SurfaceSubChunkTag convertSurfaceTagStringToEnum(string tag);

	static void parseCol12Value(char buffer[], unsigned& offset, COL12& col);
	static void parseFloatValue(char buffer[], unsigned& offset, float& fval);
	static void parseFloatVxValues(char buffer[], unsigned& offset, float& fval, unsigned& vx);
	static void parseVxValues(char buffer[], unsigned& offset, unsigned& uval);
};

