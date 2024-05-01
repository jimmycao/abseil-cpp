#include <gtest/gtest.h>

#include "log.h"
#include "hash.h"

namespace mlf {

TEST(HashTest, hash) {
    std::string str = "hello, world!";
    size_t value = Hash<std::string>::hash(str);
    MLOG(INFO) << "value: " << value;

    float pi = 3.1415926;
    size_t fvalue = Hash<float>::hash(pi);
    MLOG(INFO) << "fvalue: " << fvalue;

    EXPECT_EQ(1, 1);
}

TEST(MurMurHash, murmur) {
    uint64_t in = 1234567L;
    uint64_t out = murmur_hash(in);
    MLOG(INFO) << "murmur_hash out: " << out;
    EXPECT_EQ(1, 1);
}

TEST(MurMurHashHalf, murmur_half) {
    uint64_t in = 1234567L;
    uint64_t out = murmur_hash_half(in);
    MLOG(INFO) << "murmur_half out: " << out;
    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    int rt = RUN_ALL_TESTS();
    return rt;
}
