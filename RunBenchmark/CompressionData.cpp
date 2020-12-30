#include "CompressionData.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include "Benchmark.h"
#include "MemoryUtilities.h"

using namespace std;

#define GIGABYTE 1073741824

CompressionData::CompressionData(const std::string& inputPath)
	: _inputPath(inputPath)
{}

CompressionData::~CompressionData() {}

void CompressionData::Load(void) {

	cout << "Loading " << _inputPath << "... " << flush;

	ifstream in(_inputPath.c_str(), ios::binary | ios::ate);
	ifstream::pos_type position = in.tellg();

	_dataLen = position > GIGABYTE ? GIGABYTE : (int)position;

	_data.resize(_dataLen);
	_compressedData.resize((size_t)(1.2 * _dataLen));
	_decompressedData.resize((size_t)(1.2 * _dataLen));

	in.seekg(0, ios::beg);
	in.read(&_data[0], _dataLen);

	cout << "read " << MemoryUtilities::ToHumanReadable(_dataLen) << endl << endl;
}

void CompressionData::Compress(const CompressionAlgorithm* pAlgorithm, TimingResult& result) {

	int numBytesRemaining       = _dataLen;
	int numOutputBytesRemaining = (int)_compressedData.size();
	int numCompressedBytes      = 0;

	const char* pIn     = _data.data();
	const char* pOut    = _compressedData.data();
	const int blockSize = pAlgorithm->GetBlockSize();

	int numToReserve = _dataLen / blockSize + 1;
	result.CompressionBlockSizes.reserve(numToReserve);

	Benchmark benchmark;

	while (numBytesRemaining > 0)
	{
		if (numOutputBytesRemaining < 0)
		{
			cout << "Negative output bytes available." << endl;
			exit(1);
		}

		const int numBytesToCompress = numBytesRemaining > blockSize ? blockSize : (int)numBytesRemaining;
		const int compressedLen      = pAlgorithm->Compress(pIn, numBytesToCompress, pOut, numOutputBytesRemaining);

		result.CompressionBlockSizes.push_back(compressedLen);

		pIn                += numBytesToCompress;
		pOut               += compressedLen;
		numCompressedBytes += compressedLen;

		numBytesRemaining       -= numBytesToCompress;
		numOutputBytesRemaining -= compressedLen;
	}

	double numSeconds = benchmark.GetElapsedSeconds();
	double speed = Benchmark::GetSpeed(_dataLen, numSeconds);
	
	cout << "- compression: " << numSeconds << " s" << endl;
	cout << "- compression: " << Benchmark::ConvertSpeed(speed) << " (" << MemoryUtilities::ToHumanReadable(numCompressedBytes) << ")" << endl;
	
	result.DataLen                   = _dataLen;
	result.NumCompressionSeconds     = numSeconds;
	result.CompressionBytesPerSecond = speed;
	result.CompressionRatio          = (double)_dataLen / (double)numCompressedBytes;
}

void CompressionData::Decompress(const CompressionAlgorithm* pAlgorithm, TimingResult& result) {

	int numBytesRemaining       = (int)_compressedData.size();
	int numOutputBytesRemaining = (int)_decompressedData.size();
	int numDecompressedBytes    = 0;

	const char* pIn  = _compressedData.data();
	char* pOut       = _decompressedData.data();

	Benchmark benchmark;

	for (int blockId = 0; blockId < result.CompressionBlockSizes.size(); blockId++)
	{
		if (numOutputBytesRemaining < 0)
		{
			cout << "Negative output bytes available." << endl;
			exit(1);
		}

		const int numBytesToDecompress = result.CompressionBlockSizes[blockId];
		const int decompressedLen      = pAlgorithm->Decompress(pIn, numBytesToDecompress, pOut, numOutputBytesRemaining);

		pIn                  += numBytesToDecompress;
		pOut                 += decompressedLen;
		numDecompressedBytes += decompressedLen;

		numBytesRemaining       -= numBytesToDecompress;
		numOutputBytesRemaining -= decompressedLen;
	}

	*pOut = 0;

	double numSeconds = benchmark.GetElapsedSeconds();
	double speed = Benchmark::GetSpeed(_dataLen, numSeconds);

	cout << "- decompression: " << numSeconds << " s" << endl;
	cout << "- decompression: " << Benchmark::ConvertSpeed(speed) << " (" << MemoryUtilities::ToHumanReadable(numDecompressedBytes) << ")" << endl;

	result.NumDecompressionSeconds     = numSeconds;
	result.DecompressionBytesPerSecond = speed;
}

void CompressionData::Verify() {

	int ret = memcmp(_decompressedData.data(), _data.data(), _dataLen);

	if (ret == 0) {
		cout << "- verification: original and decompressed data is equal." << endl;
		return;
	}

	cout << "- verification: original and decompressed data ARE NOT equal." << endl;
	exit(1);
}
