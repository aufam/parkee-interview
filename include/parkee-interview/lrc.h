#ifndef PARKEE_INTERVIEW_LRC_H
#define PARKEE_INTERVIEW_LRC_H

#include <vector>
#include <cstdint>

namespace Project::parkee::lrc {
    uint8_t encode(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end);
    uint8_t encode(const std::vector<uint8_t>& data);
}

#endif

