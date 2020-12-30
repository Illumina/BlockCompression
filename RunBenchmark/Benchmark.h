#pragma once

#include <cstdint>
#include <string>

class Benchmark {
public:
	Benchmark();
	~Benchmark();
	double GetElapsedSeconds() const;

	static double GetSpeed(int numBytes, double numSeconds) { return numBytes / numSeconds; }
	static std::string ConvertSpeed(double speed);
	static std::string ConvertTime(double numSeconds);

private:
	int64_t _startTime;
	int64_t _frequency;
};
