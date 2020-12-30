#pragma once

#include <string>

class CompressionAlgorithm {
public:
	CompressionAlgorithm() {};
	virtual ~CompressionAlgorithm() {};
	virtual int Compress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const = 0;
	virtual int Decompress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const = 0;
	virtual int GetBlockSize(void) const = 0;
	virtual int GetCompressionLevel(void) const = 0;
	virtual std::string GetDescription(void) const = 0;
};
