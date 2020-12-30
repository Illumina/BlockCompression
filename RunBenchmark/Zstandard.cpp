#include "Zstandard.h"
#include "zstd.h"

using namespace std;

Zstandard::Zstandard(int compressionLevel, int blockSize)
	: _compressionLevel(compressionLevel)
	, _blockSize(blockSize)
{}

Zstandard::~Zstandard() {}

int Zstandard::Compress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const {

	const int numCompressedBytes = (int)ZSTD_compress((void*)destination, destinationLen, source, sourceLen, _compressionLevel);

	if (numCompressedBytes < 0) {
		printf("ERROR: could not execute ZSTD_compress: %d\n", numCompressedBytes);
		exit(1);
	}

	return numCompressedBytes;
}

int Zstandard::Decompress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const {
	return (int)ZSTD_decompress((void*)destination, destinationLen, source, sourceLen);
}
