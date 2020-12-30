#include "Runner.h"
#include <iostream>
#include "BenchmarkStatistics.h"

using namespace std;

Runner::Runner(vector<CompressionAlgorithm*> algorithms)
	: _algorithms(algorithms)
{}

Runner::~Runner() {}

vector<TimingResult> Runner::EvaluateAlgorithm(CompressionData& data, const CompressionAlgorithm* pAlgorithm) {

	cout << pAlgorithm->GetDescription() << " -" << pAlgorithm->GetCompressionLevel() << endl;

	const int numReplicates = 10;

	vector<TimingResult> results;
	results.reserve(numReplicates);

	for (int i = 0; i < numReplicates; i++) {
		cout << "- replicate " << i + 1 << ":" << endl;
		TimingResult result;
		data.Compress(pAlgorithm, result);
		data.Decompress(pAlgorithm, result);
		data.Verify();
		results.push_back(result);
		cout << endl;
	}

	return results;
}

void Runner::Execute(CompressionData& data) {

	BenchmarkStatistics stats;

	for (auto it = _algorithms.begin(); it != _algorithms.end(); ++it)
	{
		CompressionAlgorithm* pAlgorithm = *it;
		auto timingResults = EvaluateAlgorithm(data, pAlgorithm);
		stats.AddTimingData(pAlgorithm->GetDescription(), pAlgorithm->GetCompressionLevel(), pAlgorithm->GetBlockSize(), timingResults);
	}

	stats.DisplayHeader();
	stats.Display();
}
