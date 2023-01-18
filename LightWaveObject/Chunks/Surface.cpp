#include "Surface.h"

/// <summary>
/// Get surface color
/// </summary>
/// <returns>Color data</returns>
COL12 Surface::getColor() {
	return _color;
}

/// <summary>
/// Parse the raw chunk data
/// </summary>
void Surface::parse(char rawBuffer[], LWO_CHUNK_HEADER header) {

	// Skip the chunk header
	unsigned offset = LWO_CHUNK_DATA_OFFSET;

	// Get name
	string name = CONVERT_BYTES_TO_STRING(rawBuffer + offset);
	offset = offset + CONVERT_STRING_LENGTH(name.length());

	// Get source
	string source = CONVERT_BYTES_TO_STRING(rawBuffer + offset);
	offset = offset + CONVERT_STRING_LENGTH(source.length());

	// Read sub-chunks
	unsigned vxValue = 0; // Placeholder for unhandled vx values
	while (offset <= header.length) {

		// Read sub-chunk tag
		string subChunkTagName = CONVERT_BYTES_TO_STRING(rawBuffer + offset, 4);
		SurfaceSubChunkTag subChunkTag = LWUtils::convertSurfaceTagStringToEnum(subChunkTagName);
		offset += 4;

		// Read sub-chunk size
		short subChunkSize = CONVERT_U2_BYTES_TO_INT((rawBuffer + offset));
		offset += 2;

		// Handle sub-chunks
		switch (subChunkTag) {
			case SurfaceSubChunkTag::COLR: // Base color
				LWUtils::parseCol12Value(rawBuffer + offset, offset, _color);
				LWUtils::parseVxValues(rawBuffer + offset, offset, _envIndex);
				break;
			case SurfaceSubChunkTag::DIFF: // Diffuse
				LWUtils::parseFloatVxValues(rawBuffer + offset, offset, _diff, vxValue);
				break;
			case SurfaceSubChunkTag::LUMI: // Luminosity
				LWUtils::parseFloatVxValues(rawBuffer + offset, offset, _lumi, vxValue);
				break;
			case SurfaceSubChunkTag::SPEC: // Specular
				LWUtils::parseFloatVxValues(rawBuffer + offset, offset, _spec, vxValue);
				break;
			case SurfaceSubChunkTag::REFL: // Reflection
				LWUtils::parseFloatVxValues(rawBuffer + offset, offset, _refl, vxValue);
				break;
			case SurfaceSubChunkTag::TRAN: // Transparency
				LWUtils::parseFloatVxValues(rawBuffer + offset, offset, _tran, vxValue);
				break;
			case SurfaceSubChunkTag::TRNL: // Translucency
				LWUtils::parseFloatVxValues(rawBuffer + offset, offset, _trnl, vxValue);
				break;
			case SurfaceSubChunkTag::GLOS: // Specular glossiness
				LWUtils::parseFloatVxValues(rawBuffer + offset, offset, _glossiness, vxValue);
				break;
			case SurfaceSubChunkTag::SMAN: // Max smoothing angle
				LWUtils::parseFloatValue(rawBuffer + offset, offset, _maxSmoothingAngle);
				break;
			//case SurfaceSubChunkTag::BLOK:
			//	break;
			//case SurfaceSubChunkTag::SHRP:
			//	break;
			//case SurfaceSubChunkTag::BUMP:
			//	break;
			//case SurfaceSubChunkTag::SIDE:
			//	break;
			//case SurfaceSubChunkTag::RFOP:
			//	break;
			//case SurfaceSubChunkTag::RIMG:
			//	break;
			//case SurfaceSubChunkTag::RSAN:
			//	break;
			//case SurfaceSubChunkTag::RBLR:
			//	break;
			//case SurfaceSubChunkTag::RIND:
			//	break;
			//case SurfaceSubChunkTag::TROP:
			//	break;
			//case SurfaceSubChunkTag::TIMG:
			//	break;
			//case SurfaceSubChunkTag::TBLR:
			//	break;
			//case SurfaceSubChunkTag::CLRH:
			//	break;
			//case SurfaceSubChunkTag::CLRF:
			//	break;
			//case SurfaceSubChunkTag::ADTR:
			//	break;
			//case SurfaceSubChunkTag::GLOW:
			//	break;
			//case SurfaceSubChunkTag::LINE:
			//	break;
			//case SurfaceSubChunkTag::ALPH:
			//	break;
			//case SurfaceSubChunkTag::VCOL:
			//	break;
			case SurfaceSubChunkTag::UNKNOWN:
				offset += subChunkSize;
				break;
			default:
				offset += subChunkSize;
				break;
		}
	}
}
