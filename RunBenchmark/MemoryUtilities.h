#pragma once

#include <string>
#include <cstdint>

class MemoryUtilities {
public:
	static std::string ToHumanReadable(int64_t numBytes);
};

