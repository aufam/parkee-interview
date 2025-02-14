#include <parkee-interview/lrc.h>

using namespace Project;

uint8_t parkee::lrc::encode(std::vector<uint8_t>::const_iterator begin, std::vector<uint8_t>::const_iterator end) {
    uint8_t res = 0x00;
    for (; begin != end; ++begin) res ^= *begin;
    return res;
}

uint8_t parkee::lrc::encode(const std::vector<uint8_t>& data) {
    return encode(data.begin(), data.end());
}
