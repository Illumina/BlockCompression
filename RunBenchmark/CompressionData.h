#pragma once

#include <string>
#include <vector>
#include "CompressionAlgorithm.h"
#include "TimingResult.h"

class CompressionData {
public:
	CompressionData(const std::string& inputPath);
	~CompressionData();
	void Load(void);
	void Compress(const CompressionAlgorithm* pAlgorithm, TimingResult& result);
	void Decompress(const CompressionAlgorithm* pAlgorithm, TimingResult& result);
	void Verify();

private:
	std::string _inputPath;
	std::vector<char> _data;
	std::vector<char> _compressedData;
	std::vector<char> _decompressedData;
	int _dataLen;
};

