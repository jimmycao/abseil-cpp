#pragma once

#include <string>
#include <cstring>
#include <sstream>
#include <vector>

#include "common/log.h"

namespace mlf {

template<class... ARGS>
void format_string_append(std::string& str, const char* fmt, ARGS&& ... args) {
    int len = snprintf(NULL, 0, fmt, args...);
    MCHECK(len >= 0);
    size_t oldlen = str.length();
    str.resize(oldlen + len + 1);
    MCHECK(snprintf(&str[oldlen], (size_t)len + 1, fmt, args...) == len);
    str.resize(oldlen + len);
}

template<class... ARGS>
void format_string_append(std::string& str, const std::string& fmt, ARGS && ... args) {
    format_string_append(str, fmt.c_str(), args...);
}

template<class... ARGS>
std::string format_string(const char* fmt, ARGS && ... args) {
    std::string str;
    format_string_append(str, fmt, args...);
    return str;
}

template<class... ARGS>
std::string format_string(const std::string& fmt, ARGS && ... args) {
    return format_string(fmt.c_str(), args...);
}

// remove leading and tailing spaces
std::string trim_spaces(const std::string& str);

/**
 * As an alternative, boost provides another powerful string split weapon. e.g.,
 *
 * #include <bits/stdc++.h>
 * #include <boost/algorithm/string.hpp>
 *
 * std::vector<std::string> results;
 * std::string input = "geeks\tprogramming everyday";
 * boost::split(results, input, boost::is_any_of("\t "));
 */
std::vector<std::string> split_string(const std::string& str, char delim);


template<class BASIC_TYPE>
BASIC_TYPE string2val(const std::string & data) {
    BASIC_TYPE converted_value;
    std::stringstream ss;
    ss << std::string(data.c_str(), data.size());
    MCHECK(ss >> converted_value) << "data[" << data << "]";
    return converted_value;
}

extern uint64_t string_hash(const std::string& key);

} //namespace mlf
