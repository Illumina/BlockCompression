#include "bgzf.h"
#include <stdint.h>
#include <zlib-ng.h>
#include <memory.h>
#include <libdeflate.h>

#define BLOCK_HEADER_LENGTH 18
#define BLOCK_FOOTER_LENGTH 8

static const uint8_t g_magic[19] = "\037\213\010\4\0\0\0\0\0\377\6\0\102\103\2\0\0\0";

static inline void packInt32(uint8_t *buffer, uint32_t value)
{
	buffer[0] = value;
	buffer[1] = value >> 8;
	buffer[2] = value >> 16;
	buffer[3] = value >> 24;
}

static inline void packInt16(uint8_t *buffer, uint16_t value)
{
	buffer[0] = (uint8_t)value;
	buffer[1] = (uint8_t)(value >> 8);
}

// ===================================================================================
// use zlib-ng for compression levels 1, 8, and 9, use libdeflate for all other levels
// ===================================================================================
int bgzf_compress(const char* destination, const int destinationLen, const char* source, const int sourceLen, const int compressionLevel)
{
	int numCompressedBytes;

	if (sourceLen == 0) {
		// EOF block
		if (destinationLen < 28) return -1;
		memcpy((void*)destination, "\037\213\010\4\0\0\0\0\0\377\6\0\102\103\2\0\033\0\3\0\0\0\0\0\0\0\0\0", 28);
		return 28;
	}

	// zlib-ng still has a faster compressor for compression level 1
	if (compressionLevel < 2 || compressionLevel > 7) return bgzf_compress_zlibng(destination, destinationLen, source, sourceLen, compressionLevel);

	uint8_t* dst = (uint8_t*)destination;

	// NB levels go up to 12 here.
	struct libdeflate_compressor* z = libdeflate_alloc_compressor(compressionLevel);
	
	if (!z) return -2;

	// Raw deflate
	size_t clen = libdeflate_deflate_compress(z, source, sourceLen, dst + BLOCK_HEADER_LENGTH, destinationLen - BLOCK_HEADER_LENGTH - BLOCK_FOOTER_LENGTH);

	if (clen <= 0) {
		libdeflate_free_compressor(z);
		return -3;
	}

	numCompressedBytes = (int)clen + BLOCK_HEADER_LENGTH + BLOCK_FOOTER_LENGTH;

	libdeflate_free_compressor(z);

	// write the header
	memcpy(dst, g_magic, BLOCK_HEADER_LENGTH); // the last two bytes are a place holder for the length of the block
	packInt16(&dst[16], numCompressedBytes - 1); // write the compressed length; -1 to fit 2 bytes

	// write the footer
	uint32_t crc = libdeflate_crc32(0, source, sourceLen);
	packInt32((uint8_t*)&dst[numCompressedBytes - 8], crc);
	packInt32((uint8_t*)&dst[numCompressedBytes - 4], sourceLen);

	return numCompressedBytes;
}

int bgzf_compress_zlibng(const char* destination, const int destinationLen, const char* source, const int sourceLen, const int compressionLevel)
{
	uint32_t crc;
	zng_stream zs;
	uint8_t* dst = (uint8_t*)destination;
	int numCompressedBytes;

	// compress the body
	zs.zalloc = NULL;
	zs.zfree = NULL;
	zs.msg = NULL;
	zs.next_in = (Bytef*)source;
	zs.avail_in = sourceLen;
	zs.next_out = dst + BLOCK_HEADER_LENGTH;
	zs.avail_out = destinationLen - BLOCK_HEADER_LENGTH - BLOCK_FOOTER_LENGTH;
	int ret = zng_deflateInit2(&zs, compressionLevel, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY); // -15 to disable zlib header/footer

	if (ret != Z_OK) return -1;
	if ((ret = zng_deflate(&zs, Z_FINISH)) != Z_STREAM_END) return -2;
	if ((ret = zng_deflateEnd(&zs)) != Z_OK) return -3;

	numCompressedBytes = (int)zs.total_out + BLOCK_HEADER_LENGTH + BLOCK_FOOTER_LENGTH;

	// write the header
	memcpy(dst, g_magic, BLOCK_HEADER_LENGTH); // the last two bytes are a place holder for the length of the block
	packInt16(&dst[16], numCompressedBytes - 1); // write the compressed length; -1 to fit 2 bytes

	// write the footer
	crc = zng_crc32(zng_crc32(0L, NULL, 0L), (Bytef*)source, sourceLen);
	packInt32((uint8_t*)&dst[numCompressedBytes - 8], crc);
	packInt32((uint8_t*)&dst[numCompressedBytes - 4], sourceLen);

	return numCompressedBytes;
}

int bgzf_decompress(const char* destination, const int destinationLen, const char* source, const int sourceLen)
{
	size_t numDecompressedBytes;

	struct libdeflate_decompressor* z = libdeflate_alloc_decompressor();
	if (!z) return -1;

	int ret = libdeflate_deflate_decompress(z, source + BLOCK_HEADER_LENGTH, sourceLen - BLOCK_HEADER_LENGTH, (void*)destination, destinationLen, &numDecompressedBytes);
	libdeflate_free_decompressor(z);
	if (ret != LIBDEFLATE_SUCCESS) return -2;

	return (int)numDecompressedBytes;
}
