#pragma once

#include "Chunk.h"
#include "../LWUtils.h"

class Surface : public Chunk {
public:

	// Structures
	struct COLOR {
		float r;
		float g;
		float b;
	};

	// Constructor
	Surface() : Chunk(ChunkTag::SURF) { }

	// Getters
	COLOR getColor();
	COL12 getCol12Color();

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private methods

	// Private data
	COL12 _color {};
	float _diff {1.0f};
	float _lumi {};
	float _spec {};
	float _refl {};
	float _tran {};
	float _trnl {};
	float _glossiness {};
	float _sharpness {};
	float _bump {};
	float _maxSmoothingAngle {-1.0f};
	unsigned _envIndex {};
};

