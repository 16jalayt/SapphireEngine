// lzss.cpp --- decompress LZSS data

#include "lzss.hpp"

#include <array>

namespace Her {
	std::vector<uint8_t> dec_LZSS(std::vector<uint8_t> fData) {
		std::array<uint8_t, 4096> BUFFER; BUFFER.fill(0x20);
		std::vector<uint8_t> output;

		uint8_t flags8;
		uint16_t writeidx = 0xFEE; // BUFFER write index
		uint16_t readidx;          // BUFFER read index
		unsigned int fidx = 0;     // index into fData

		while (fidx < fData.size()) {
			flags8 = fData.at(fidx);
			fidx++;
			for (int i = 0; i < 8; i++) {
				if (flags8 & 1) { // literal byte
					output.push_back(fData.at(fidx));
					BUFFER.at(writeidx) = fData.at(fidx);
					writeidx++; writeidx %= 4096;
					fidx++;
				}
				else { // run data, run!
					readidx = fData.at(fidx);
					fidx++;
					readidx |= (fData.at(fidx) & 0xF0) << 4;
					for (int j = 0; j < (fData.at(fidx) & 0x0F) + 3; j++) {
						output.push_back(BUFFER.at(readidx));
						BUFFER.at(writeidx) = BUFFER.at(readidx);
						readidx++; readidx %= 4096;
						writeidx++; writeidx %= 4096;
					}
					fidx++;
				}
				flags8 >>= 1;
				if (fidx >= fData.size()) break;
			}
		}

		return output;
	}
}