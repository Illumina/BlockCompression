#pragma once

#include "CompressionAlgorithm.h"
#include <sstream>

class Zstandard : public CompressionAlgorithm {
public:
	Zstandard(int compressionLevel, int blockSize);
	~Zstandard();
	int Compress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const;
	int Decompress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const;
	int GetBlockSize(void) const { return _blockSize; }
	int GetCompressionLevel(void) const { return _compressionLevel; }
	std::string GetDescription(void) const { return "Zstd"; }

private:
	int _blockSize;
	int _compressionLevel;
};

