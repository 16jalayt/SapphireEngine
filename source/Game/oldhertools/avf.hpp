// avf.hpp --- various things concerning just AVF files

#pragma once

#include <cstdint>
#include <fstream>
#include <vector>

namespace Her {
	struct AVF_idxEntry {
		uint16_t frameNo = 0;
		uint32_t startAt = 0;
		uint32_t goFor = 0;
	};

	std::vector<Her::AVF_idxEntry> getAVFindex(std::ifstream& theavf, uint16_t numEntries);
}