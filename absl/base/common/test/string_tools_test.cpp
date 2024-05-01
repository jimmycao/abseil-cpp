#include <gtest/gtest.h>

#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>

#include "log.h"
#include "string_tools.h"

namespace mlf {

TEST(StringToolsTest, format_string) {
    std::string result = format_string("PI=%.7f", 3.1415926);
    EXPECT_EQ("PI=3.1415926", result);

    std::string format = "PI=%.2f";
    EXPECT_EQ("PI=3.14", format_string(format, 3.1415926));
}

TEST(StringToolsTest, format_string_append) {
    std::string result = "PI=";
    format_string_append(result, "%.7f", 3.1415926);
    EXPECT_EQ("PI=3.1415926", result);
}

TEST(StringToolsTest, trim_spaces) {
    std::string test = " hello, world!    ";
    std::string after_trim = trim_spaces(test);
    EXPECT_EQ("hello, world!", after_trim);
}

TEST(StringToolsTest, split_string) {
    std::vector<std::string> results = split_string("geeks\tprogramming\teveryday!", '\t');
    std::vector<std::string> expects = {"geeks", "programming", "everyday!"};
    EXPECT_EQ(expects, results);
}

TEST(StringToolsTest, string2val) {
    EXPECT_EQ(99, string2val<int>("99"));
}

TEST(StringToolsTest, string_hash) {
    std::string key = "hello, world!";
    uint64_t result = string_hash(key);
    MLOG(INFO) << "after string_hash, result: " << result;
    EXPECT_EQ(1251562168029011191, result);
}


TEST(StringToolsTest, boost_split) {
    std::vector<std::string> results;
    std::string input = "geeks\tprogramming everyday";
    boost::split(results, input, boost::is_any_of("\t "));

    std::vector<std::string> expects = {"geeks", "programming", "everyday"};
    EXPECT_EQ(expects, results);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
