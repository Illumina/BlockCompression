#pragma once

#include <string>
#include <vector>
#include "TimingResult.h"
#include "SummaryStatistics.h"
#include "BenchmarkResult.h"

class BenchmarkStatistics {
public:
	BenchmarkStatistics();
	~BenchmarkStatistics();
	void AddTimingData(const std::string description, const int compressionLevel, const int blockSize, const std::vector<TimingResult>& results);
	void Display(void);
	void DisplayHeader(void);

private:
	std::vector<BenchmarkResult> _benchmarkResults;
	SummaryStatistics CalculateSummaryStatistics(const std::vector<TimingResult>& results);
	double Mean(const std::vector<double>& values);
	int GetMaxFieldSize(const size_t fieldSize, int maxSize);
	void DisplayField(const std::string& field, const int fieldSize, const char leftSpace, const char rightSpace);
	void SetMinFieldLengths(void);
	void SetRankAndScale(void);
	int _algorithmLen;
	int _totalTimeLen;
	int _compSpeedLen;
	int _decompSpeedLen;
};

