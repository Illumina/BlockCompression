#pragma once

#include <vector>

class TimingResult {
public:
	int DataLen;
	double CompressionRatio;
	double NumCompressionSeconds;
	double NumDecompressionSeconds;
	double CompressionBytesPerSecond;
	double DecompressionBytesPerSecond;
	std::vector<int> CompressionBlockSizes;
};
