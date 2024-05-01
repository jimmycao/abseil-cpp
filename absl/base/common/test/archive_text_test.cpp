#include <gtest/gtest.h>

#include "log.h"
#include "archive_text.h"

namespace mlf {

TEST(TextArchiveTest, archive_int) {
    TextArchive ar;
    int a = 1;
    ar << a;

    int b;
    ar >> b;
    EXPECT_EQ(a, b);
}

TEST(TextArchiveTest, archive_tuple) {
    TextArchive ar;
    std::tuple<int, float, std::string> t1 = std::make_tuple(1, 0.1, "str");
    ar << t1;

    std::tuple<int, float, std::string> t2; 
    ar >> t2;
    EXPECT_EQ(t1, t2);
}

TEST(TextArchiveTest, archive_string) {
    TextArchive ar;

    std::string orig_str = "hello";
    ar << orig_str;

    std::string target_str;
    ar >> target_str;
    EXPECT_EQ(orig_str, target_str);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    int rt = RUN_ALL_TESTS();
    return rt;
}
