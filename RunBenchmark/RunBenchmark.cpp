#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Zlib.h"
#include "Zstandard.h"
#include "ZstandardDict.h"
#include "Runner.h"
#include "bgzf.h"

using namespace std;

#define SA_BLOCK_SIZE 16777216

vector<CompressionAlgorithm*> GetCompressionAlgorithms(vector<char> dictBuffer) {

	vector<CompressionAlgorithm*> algorithms;

	algorithms.push_back(new Zlib(1, BGZF_MAX_BLOCK_SIZE));
	//algorithms.push_back(new Zlib(2, BGZF_MAX_BLOCK_SIZE));
	//algorithms.push_back(new Zlib(3, BGZF_MAX_BLOCK_SIZE));
	//algorithms.push_back(new Zlib(4, BGZF_MAX_BLOCK_SIZE));
	algorithms.push_back(new Zlib(5, BGZF_MAX_BLOCK_SIZE));
	//algorithms.push_back(new Zlib(6, BGZF_MAX_BLOCK_SIZE));
	//algorithms.push_back(new Zlib(7, BGZF_MAX_BLOCK_SIZE));
	//algorithms.push_back(new Zlib(8, BGZF_MAX_BLOCK_SIZE));
	algorithms.push_back(new Zlib(9, BGZF_MAX_BLOCK_SIZE));

	algorithms.push_back(new Zstandard(1, SA_BLOCK_SIZE));
	algorithms.push_back(new Zstandard(17, SA_BLOCK_SIZE));

	char* pDict  = dictBuffer.data();
	int dictSize = (int)dictBuffer.size();
	
	algorithms.push_back(new ZstandardDict(1, SA_BLOCK_SIZE, pDict, dictSize));
	algorithms.push_back(new ZstandardDict(17, SA_BLOCK_SIZE, pDict, dictSize));

	return algorithms;
}

vector<char> LoadDictionary(string dictPath) {

	ifstream ifs(dictPath.c_str(), ios::binary | ios::ate);
	ifstream::pos_type bufferSize = ifs.tellg();

	vector<char> buffer(bufferSize);

	ifs.seekg(0, ios::beg);
	ifs.read(&buffer[0], bufferSize);
	return buffer;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("USAGE: %s <input_path> <input_dict>\n", argv[0]);
		exit(1);
	}

	string inputPath = argv[1];
	string dictPath  = argv[2];

	CompressionData data(inputPath);
	data.Load();

	auto dictBuffer = LoadDictionary(dictPath);
	auto algorithms = GetCompressionAlgorithms(dictBuffer);

	Runner runner(algorithms);
	runner.Execute(data);

	return 0;
}
