#include "ul_sign.h"
#include "string_tools.h"

namespace mlf {

// remove leading and tailing spaces
std::string trim_spaces(const std::string& str) {
    const char* p = str.c_str();

    while (*p != 0 && isspace(*p)) {
        p++;
    }
    size_t len = strlen(p);
    while (len > 0 && isspace(p[len - 1])) {
        len--;
    }

    return std::string(p, len);
}

std::vector<std::string> split_string(const std::string& str, char delim) {
    const char* p;

    size_t num = 1;
    for (p = str.c_str(); *p != 0; p++) {
        if (*p == delim) {
            num++;
        }
    }

    std::vector<std::string> list(num);

    const char* last = str.c_str();
    num = 0;
    for (p = str.c_str(); *p != 0; p++) {
        if (*p == delim) {
            list[num++] = std::string(last, p - last);
            last = p + 1;
        }
    }
    list[num] = std::string(last, p - last);

    return list;
};

uint64_t string_hash(const std::string& key) {
    uint32_t tmp[2];
    creat_sign_fs64((char*)key.c_str(), key.size(), &tmp[0], &tmp[1]);
    uint64_t output = (((uint64_t)tmp[0]) << 32) | tmp[1];
    return output;
}

} // namepace mlf
