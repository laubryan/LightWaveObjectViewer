#pragma once

#include "Chunk.h"
#include "../LWUtils.h"

class Surface : public Chunk {
public:

	// Constructor
	Surface() : Chunk(ChunkTag::SURF) { }

	// Public methods
	void parse(char rawBuffer[], LWO_CHUNK_HEADER header) override;

private:

	// Private methods
	float parseFloatValue(char buffer[], unsigned& offset);
	float parseFloatVxValues(char buffer[], unsigned& offset, unsigned& vx);

	// Private data
	COL12 _color;
	float _diff;
	float _lumi;
	float _spec;
	float _refl;
	float _tran;
	float _trnl;
	float _glossiness;
	float _sharpness;
	float _bump;
	unsigned _envIndex;
};

