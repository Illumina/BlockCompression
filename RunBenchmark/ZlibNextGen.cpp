#include "ZlibNextGen.h"
#include "bgzf.h"

using namespace std;

ZlibNextGen::ZlibNextGen(int compressionLevel, int blockSize)
	: _compressionLevel(compressionLevel)
	, _blockSize(blockSize)
{}

ZlibNextGen::~ZlibNextGen() {}

int ZlibNextGen::Compress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const {

	const int numCompressedBytes = bgzf_compress(destination, destinationLen, source, sourceLen, _compressionLevel);

	if (numCompressedBytes < 0) {
		printf("ERROR: could not execute bgzf_compress: %d\n", numCompressedBytes);
		exit(1);
	}

	return numCompressedBytes;
}

int ZlibNextGen::Decompress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const {
	return bgzf_decompress((char*)destination, destinationLen, (char*)source, sourceLen);
}
