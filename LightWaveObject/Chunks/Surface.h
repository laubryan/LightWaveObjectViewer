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
	COL12 color_;
	float diff_;
	float lumi_;
	float spec_;
	float refl_;
	float tran_;
	float trnl_;
	float glossiness_;
	float sharpness_;
	float bump_;
	unsigned envIndex_;
};

