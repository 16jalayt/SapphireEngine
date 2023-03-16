// crypt_misc.hpp --- miscellaneous crypto functions

#pragma once

#include <vector>
#include <cstdint>

namespace Her {
    std::vector<uint8_t> sub_offsets(char * fData, uint32_t fSize);
}
