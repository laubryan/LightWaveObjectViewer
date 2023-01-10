#pragma once
#include "Chunks/ChunkDefinitions.h"

class LWUtils {
public:
	static ChunkTag convertTagStringTagToEnum(string tag);
	static string convertTagEnumToString(ChunkTag tagEnum);

	static SurfaceSubChunkTag convertSurfaceTagStringToEnum(string tag);
};

