// avf/fileidx.cpp --- gather file index in AVF files

#include "avf.hpp"

namespace Her {
    std::vector<Her::AVF_idxEntry> getAVFindex(std::ifstream & theavf, uint16_t numEntries) {
        theavf.seekg(0x21);
        char * rawData = new char[19];

        std::vector<Her::AVF_idxEntry> jumpList(numEntries);

        for (int i=0;i<numEntries;i++) {
            theavf.read(rawData, 19);
            uint16_t entNum = ((uint16_t*)rawData)[0];
            jumpList.at(entNum).frameNo = entNum;
            jumpList.at(entNum).startAt = ((uint32_t*)(rawData+2))[0];
            jumpList.at(entNum).goFor   = ((uint32_t*)(rawData+2))[1];
        }

        delete[] rawData;
        return jumpList;
    }
}
