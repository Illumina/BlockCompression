#include "ZstandardDict.h"
#include "zstd.h"

using namespace std;

ZstandardDict::ZstandardDict(int compressionLevel, int blockSize, char* dictBuffer, int dictSize)
	: _compressionLevel(compressionLevel)
	, _blockSize(blockSize)
{
	_compressDict      = ZSTD_createCDict(dictBuffer, dictSize, compressionLevel);
	_decompressDict    = ZSTD_createDDict(dictBuffer, dictSize);
	_compressContext   = ZSTD_createCCtx();
	_decompressContext = ZSTD_createDCtx();
}

ZstandardDict::~ZstandardDict() {}

int ZstandardDict::Compress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const {

	int numCompressedBytes = (int)ZSTD_compress_usingCDict(_compressContext, (void*)destination, destinationLen, source, sourceLen, _compressDict);

	if (numCompressedBytes < 0) {
		printf("ERROR: could not execute ZSTD_compress: %d\n", numCompressedBytes);
		exit(1);
	}

	return numCompressedBytes;
}

int ZstandardDict::Decompress(const char* source, const int sourceLen, const char* destination, const int destinationLen) const {
	return (int)ZSTD_decompress_usingDDict(_decompressContext, (void*)destination, destinationLen, source, sourceLen, _decompressDict);
}
