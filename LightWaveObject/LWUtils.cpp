#include "LWUtils.h"

/// <summary>
/// Convert chunk tag string to equivalent enum
/// </summary>
/// <param name="tag">Tag ID</param>
/// <returns>Tag enum</returns>
ChunkTag LWUtils::convertTagStringTagToEnum(string tag) {
	if(tag=="LAYR") return ChunkTag::LAYR; // 0
	if(tag=="PNTS") return ChunkTag::PNTS; // 1
	if(tag=="VMAP") return ChunkTag::VMAP; // 2
	if(tag=="POLS") return ChunkTag::POLS; // 3
	if(tag=="TAGS") return ChunkTag::TAGS; // 4
	if(tag=="PTAG") return ChunkTag::PTAG; // 5
	if(tag=="VMAD") return ChunkTag::VMAD; // 6
	if(tag=="VMPA") return ChunkTag::VMPA; // 7
	if(tag=="ENVL") return ChunkTag::ENVL; // 8
	if(tag=="CLIP") return ChunkTag::CLIP; // 9
	if(tag=="SURF") return ChunkTag::SURF; // 10
	if(tag=="BBOX") return ChunkTag::BBOX; // 11
	if(tag=="DESC") return ChunkTag::DESC; // 12
	if(tag=="TEXT") return ChunkTag::TEXT; // 13
	if(tag=="ICON") return ChunkTag::ICON; // 14

	return ChunkTag::UNKNOWN;
}

/// <summary>
/// Convert chunk tag enum to equivalent string
/// </summary>
/// <param name="tag">Tag enum ID</param>
/// <returns>Tag string</returns>
string LWUtils::convertTagEnumToString(ChunkTag tagEnum) {
	if(tagEnum==ChunkTag::LAYR) return "LAYR"; // 0
	if(tagEnum==ChunkTag::PNTS) return "PNTS"; // 1
	if(tagEnum==ChunkTag::VMAP) return "VMAP"; // 2
	if(tagEnum==ChunkTag::POLS) return "POLS"; // 3
	if(tagEnum==ChunkTag::TAGS) return "TAGS"; // 4
	if(tagEnum==ChunkTag::PTAG) return "PTAG"; // 5
	if(tagEnum==ChunkTag::VMAD) return "VMAD"; // 6
	if(tagEnum==ChunkTag::VMPA) return "VMPA"; // 7
	if(tagEnum==ChunkTag::ENVL) return "ENVL"; // 8
	if(tagEnum==ChunkTag::CLIP) return "CLIP"; // 9
	if(tagEnum==ChunkTag::SURF) return "SURF"; // 10
	if(tagEnum==ChunkTag::BBOX) return "BBOX"; // 11
	if(tagEnum==ChunkTag::DESC) return "DESC"; // 12
	if(tagEnum==ChunkTag::TEXT) return "TEXT"; // 13
	if(tagEnum==ChunkTag::ICON) return "ICON"; // 14

	return "UNKNOWN";
}

/// <summary>
/// Convert surface sub-chunk tag to equivalent enum
/// </summary>
/// <param name="tag">Tag string</param>
/// <returns>Tag enum</returns>
SurfaceSubChunkTag LWUtils::convertSurfaceTagStringToEnum(string tag) {

	if (tag == "COLR") return SurfaceSubChunkTag::COLR; // 0
	if (tag == "DIFF") return SurfaceSubChunkTag::DIFF; // 1
	if (tag == "LUMI") return SurfaceSubChunkTag::LUMI; // 2
	if (tag == "SPEC") return SurfaceSubChunkTag::SPEC; // 3
	if (tag == "REFL") return SurfaceSubChunkTag::REFL; // 4
	if (tag == "TRAN") return SurfaceSubChunkTag::TRAN; // 5
	if (tag == "TRNL") return SurfaceSubChunkTag::TRNL; // 6
	if (tag == "GLOS") return SurfaceSubChunkTag::GLOS; // 7
	if (tag == "BLOK") return SurfaceSubChunkTag::BLOK; // 8
	if (tag == "SHRP") return SurfaceSubChunkTag::SHRP; // 9
	if (tag == "BUMP") return SurfaceSubChunkTag::BUMP; // 10
	if (tag == "SIDE") return SurfaceSubChunkTag::SIDE; // 11
	if (tag == "SMAN") return SurfaceSubChunkTag::SMAN; // 12
	if (tag == "RFOP") return SurfaceSubChunkTag::RFOP; // 13
	if (tag == "RIMG") return SurfaceSubChunkTag::RIMG; // 14
	if (tag == "RSAN") return SurfaceSubChunkTag::RSAN; // 15
	if (tag == "RBLR") return SurfaceSubChunkTag::RBLR; // 16
	if (tag == "RIND") return SurfaceSubChunkTag::RIND; // 17
	if (tag == "TROP") return SurfaceSubChunkTag::TROP; // 18
	if (tag == "TIMG") return SurfaceSubChunkTag::TIMG; // 19
	if (tag == "TBLR") return SurfaceSubChunkTag::TBLR; // 20
	if (tag == "CLRH") return SurfaceSubChunkTag::CLRH; // 21
	if (tag == "CLRF") return SurfaceSubChunkTag::CLRF; // 22
	if (tag == "ADTR") return SurfaceSubChunkTag::ADTR; // 23
	if (tag == "GLOW") return SurfaceSubChunkTag::GLOW; // 24
	if (tag == "LINE") return SurfaceSubChunkTag::LINE; // 25
	if (tag == "ALPH") return SurfaceSubChunkTag::ALPH; // 26
	if (tag == "VCOL") return SurfaceSubChunkTag::VCOL; // 27

	return SurfaceSubChunkTag::UNKNOWN;
}

/// <summary>
/// Parse float value from buffer and advance offset
/// </summary>
/// <param name="buffer">Raw buffer</param>
/// <param name="offset">Current offset</param>
/// <returns>Retrieved float</returns>
void LWUtils::parseFloatValue(char buffer[], unsigned& offset, float& fval) {
	fval = CONVERT_FLOAT_BYTES(buffer);
	offset += 4;
}

/// <summary>
/// Parse COL12 value from buffer and advance offset
/// </summary>
/// <param name="buffer">Raw buffer</param>
/// <param name="offset">Current offset</param>
/// <returns>Retrieved COL12 value</returns>
void LWUtils::parseCol12Value(char buffer[], unsigned& offset, COL12& col) {
	col = CONVERT_COL12_BYTES(buffer);
	offset += sizeof(COL12);
}

/// <summary>
/// Parse float and vx values from buffer and advance offset
/// </summary>
/// <param name="buffer">Raw buffer</param>
/// <param name="offset">Current offset</param>
/// <returns>Retrieved unsigned int</returns>
void LWUtils::parseFloatVxValues(char buffer[], unsigned& offset, float& fval, unsigned& vx) {

	// Float value
	fval = CONVERT_FLOAT_BYTES(buffer);
	offset += sizeof(float);

	// VX value
	LWUtils::parseVxValues(buffer + sizeof(float), offset, vx);
}

/// <summary>
/// Parse VX value from buffer and advance offset
/// </summary>
/// <param name="buffer">Raw buffer</param>
/// <param name="offset">Current offset</param>
/// <param name="vx"></param>
/// <returns></returns>
void LWUtils::parseVxValues(char buffer[], unsigned& offset, unsigned& uval) {

	if (buffer[0] == 255) {
		// Four byte index
		uval = CONVERT_U4_BYTES_TO_INT(buffer);
		offset += 4;
	}
	else {
		// Two byte index
		uval = CONVERT_U2_BYTES_TO_INT(buffer);
		offset += 2;
	}
}