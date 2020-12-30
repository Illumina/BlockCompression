#pragma once

#include "CompressionAlgorithm.h"
#include <sstream>
#include <zstd.h>

class ZstandardDict : public CompressionAlgorithm {
public:
	ZstandardDict(int compressionLevel, int blockSize, char* dictBuffer, int dictSize);
	~ZstandardDict();
	int Compress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const;
	int Decompress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const;
	int GetBlockSize(void) const { return _blockSize; }
	int GetCompressionLevel(void) const { return _compressionLevel; }
	std::string GetDescription(void) const { return "ZstdDict"; }

private:
	int _blockSize;
	int _compressionLevel;
	ZSTD_CDict* _compressDict;
	ZSTD_DDict* _decompressDict;
	ZSTD_CCtx* _compressContext;
	ZSTD_DCtx* _decompressContext;
};

