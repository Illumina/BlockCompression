#pragma once

#include "CompressionAlgorithm.h"
#include <sstream>

class Zlib : public CompressionAlgorithm {
public:
	Zlib(int compressionLevel, int blockSize);
	~Zlib();
	int Compress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const;
	int Decompress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const;
	int GetBlockSize(void) const { return _blockSize; }
	int GetCompressionLevel(void) const { return _compressionLevel; }
	std::string GetDescription(void) const { return "Zlib"; }

private:
	int _blockSize;
	int _compressionLevel;
};

