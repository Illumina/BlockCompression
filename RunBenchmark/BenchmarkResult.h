#pragma once

#include <string>
#include "SummaryStatistics.h"
#include "Benchmark.h"

class BenchmarkResult {
public:
	BenchmarkResult(std::string description, const int compressionLevel, const int blockSize, const SummaryStatistics& ss, const int dataLen)
		: Description(description)
		, CompressionLevel(compressionLevel)
		, BlockSize(blockSize)
		, CompressionRatio(ss.CompressionRatio)
		, CompressionSpeed(ss.CompressionSpeed)
		, DecompressionSpeed(ss.DecompressionSpeed)
		, NumCompressionSeconds(ss.NumCompressionSecondsMean)
		, NumDecompressionSeconds(ss.NumDecompressionSecondsMean)
		, CompressionSpeedMeanString(Benchmark::ConvertSpeed(ss.CompressionSpeed))
		, DecompressionSpeedMeanString(Benchmark::ConvertSpeed(ss.DecompressionSpeed))
		, TotalTimeString(Benchmark::ConvertTime(ss.NumCompressionSecondsMean + ss.NumDecompressionSecondsMean))
		, SpeedScale(0.0)
		, CompressionRatioScale(0.0)
		, SpeedRank(0)
		, CompressionRatioRank(0)
	{
		double numTotalSeconds = NumCompressionSeconds + NumDecompressionSeconds;
		TotalSpeed             = dataLen / numTotalSeconds;
		TotalTimeString        = Benchmark::ConvertTime(numTotalSeconds);
	}

	~BenchmarkResult() {}

	std::string Description;
	int CompressionLevel;
	int BlockSize;

	double NumCompressionSeconds;
	double NumDecompressionSeconds;
	double TotalSpeed;

	double CompressionRatio;
	double CompressionSpeed;
	double DecompressionSpeed;
	
	double SpeedScale;
	double CompressionRatioScale;

	int SpeedRank;
	int CompressionRatioRank;

	std::string CompressionSpeedMeanString;
	std::string DecompressionSpeedMeanString;
	std::string TotalTimeString;
};
