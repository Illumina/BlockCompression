#include "MemoryUtilities.h"
#include <sstream>
#include <iomanip>

using namespace std;

#define NumBytesInGB 1073741824
#define NumBytesInMB 1048576
#define NumBytesInKB 1024

string MemoryUtilities::ToHumanReadable(int64_t numBytes) {

	stringstream ss;

	if (numBytes > NumBytesInGB)
	{
		double gigaBytes = numBytes / (double)NumBytesInGB;
		ss << fixed << setprecision(3) << gigaBytes << " GB";
		return ss.str();
	}

	if (numBytes > NumBytesInMB)
	{
		double megaBytes = numBytes / (double)NumBytesInMB;
		ss << fixed << setprecision(1) << megaBytes << " MB";
		return ss.str();
	}

	if (numBytes > NumBytesInKB)
	{
		double kiloBytes = numBytes / (double)NumBytesInKB;
		ss << fixed << setprecision(1) << kiloBytes << " KB";
		return ss.str();
	}

	ss << numBytes << " B";
	return ss.str();
}