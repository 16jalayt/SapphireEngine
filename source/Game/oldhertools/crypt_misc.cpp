// crypt_misc.cpp --- miscellaneous crypto functions

#include "crypt_misc.hpp"

namespace Her {
	std::vector<uint8_t> sub_offsets(char* fData, uint32_t fSize) {
		std::vector<uint8_t> output(fSize);

		for (uint32_t i = 0; i < fSize; i++) {
			output.at(i) = (uint8_t)fData[i] - i;
		}

		return output;
	}
}