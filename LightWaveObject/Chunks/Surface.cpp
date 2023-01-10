#include "Surface.h"

/// <summary>
/// Parse the raw chunk data
/// </summary>
void Surface::parse(char rawBuffer[], LWO_CHUNK_HEADER header) {

	// TODO: Complete the surface parser before using
	return;

	// Skip the chunk header
	unsigned offset = LWO_CHUNK_DATA_OFFSET;

	// Get name
	string name = CONVERT_BYTES_TO_STRING(rawBuffer + offset);
	offset = offset + CONVERT_STRING_LENGTH(name.length());

	// Get source
	string source = CONVERT_BYTES_TO_STRING(rawBuffer + offset);
	offset = offset + CONVERT_STRING_LENGTH(source.length());

	// Read sub-chunks
	unsigned unhandledIndex = 0; // Placeholder for unhandled vx values
	while (offset <= header.length) {

		// Read sub-chunk tag
		string subChunkTagName = CONVERT_BYTES_TO_STRING(rawBuffer + offset, 4);
		SurfaceSubChunkTag subChunkTag = LWUtils::convertSurfaceTagStringToEnum(subChunkTagName);
		offset += 4;

		// Handle sub-chunks
		switch (subChunkTag) {
			case SurfaceSubChunkTag::COLR:
				color_ = CONVERT_COL12_BYTES(rawBuffer + offset);
				offset += sizeof(COL12);
				envIndex_ = CONVERT_VX_BYTES(rawBuffer + offset);
				offset += 4;
				break;
			case SurfaceSubChunkTag::DIFF:
				diff_ = parseFloatVxValues(rawBuffer, offset, unhandledIndex);
				break;
			case SurfaceSubChunkTag::LUMI:
				lumi_= parseFloatVxValues(rawBuffer, offset, unhandledIndex);
				break;
			case SurfaceSubChunkTag::SPEC:
				spec_= parseFloatVxValues(rawBuffer, offset, unhandledIndex);
				break;
			case SurfaceSubChunkTag::REFL:
				refl_= parseFloatVxValues(rawBuffer, offset, unhandledIndex);
				break;
			case SurfaceSubChunkTag::TRAN:
				tran_= parseFloatVxValues(rawBuffer, offset, unhandledIndex);
				break;
			case SurfaceSubChunkTag::TRNL:
				trnl_= parseFloatVxValues(rawBuffer, offset, unhandledIndex);
				break;
			case SurfaceSubChunkTag::GLOS:
				glossiness_ = parseFloatVxValues(rawBuffer, offset, unhandledIndex);
				break;
			case SurfaceSubChunkTag::BLOK:
				break;
			case SurfaceSubChunkTag::SHRP:
				break;
			case SurfaceSubChunkTag::BUMP:
				break;
			case SurfaceSubChunkTag::SIDE:
				break;
			case SurfaceSubChunkTag::SMAN:
				break;
			case SurfaceSubChunkTag::RFOP:
				break;
			case SurfaceSubChunkTag::RIMG:
				break;
			case SurfaceSubChunkTag::RSAN:
				break;
			case SurfaceSubChunkTag::RBLR:
				break;
			case SurfaceSubChunkTag::RIND:
				break;
			case SurfaceSubChunkTag::TROP:
				break;
			case SurfaceSubChunkTag::TIMG:
				break;
			case SurfaceSubChunkTag::TBLR:
				break;
			case SurfaceSubChunkTag::CLRH:
				break;
			case SurfaceSubChunkTag::CLRF:
				break;
			case SurfaceSubChunkTag::ADTR:
				break;
			case SurfaceSubChunkTag::GLOW:
				break;
			case SurfaceSubChunkTag::LINE:
				break;
			case SurfaceSubChunkTag::ALPH:
				break;
			case SurfaceSubChunkTag::VCOL:
				break;
			case SurfaceSubChunkTag::UNKNOWN:
				break;
			default:
				break;
		}
	}
}

/// <summary>
/// Parse float value from buffer and advance offset
/// </summary>
/// <param name="buffer">Raw buffer</param>
/// <param name="offset">Current offset</param>
/// <returns>Retrieved float</returns>
float Surface::parseFloatValue(char buffer[], unsigned& offset) {
	float val = CONVERT_FLOAT_BYTES(buffer + offset);
	offset += 4;
	return val;
}

/// <summary>
/// Parse float and vx values from buffer and advance offset
/// </summary>
/// <param name="buffer">Raw buffer</param>
/// <param name="offset">Current offset</param>
/// <returns>Retrieved float</returns>
float Surface::parseFloatVxValues(char buffer[], unsigned& offset, unsigned& vx) {

	// Float value
	float floatVal = CONVERT_FLOAT_BYTES(buffer + offset);
	offset += sizeof(float);

	// Vx value
	vx = CONVERT_VX_BYTES(buffer + offset);
	offset += 4; // CONVERT_VX_LENGTH(envIndex_);

	return floatVal;
}
