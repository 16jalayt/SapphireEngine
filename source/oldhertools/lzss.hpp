// lzss.hpp --- header for LZSS handling

#pragma once

#include <vector>

#include <cstdint>

namespace Her {
    std::vector<uint8_t> dec_LZSS(std::vector<uint8_t> fData);
}
