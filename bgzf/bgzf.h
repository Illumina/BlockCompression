#pragma once

#define BGZF_MAX_BLOCK_SIZE 0x10000

#ifdef __cplusplus
extern "C" {
#endif

	int bgzf_compress(const char* destination, const int destinationLen, const char* source, const int sourceLen, const int compressionLevel);
	int bgzf_compress_zlibng(const char* destination, const int destinationLen, const char* source, const int sourceLen, const int compressionLevel);
	int bgzf_decompress(const char* destination, const int destinationLen, const char* source, const int sourceLen);

#ifdef __cplusplus
}
#endif
