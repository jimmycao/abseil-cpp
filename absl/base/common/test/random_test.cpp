#include <functional>

#include <gtest/gtest.h>

#include "log.h"
#include "random.h"

namespace mlf {

TEST(RandomTest, uniform) {
    float start = 0.0;
    float end = 10.0;

    float result = Random::instance().uniform<float>(start, end);
    MLOG(INFO) << "uniform, result: " << result;

    int num = 10;
    std::vector<float> results = Random::instance().uniform<float>(start, end, num);
    for (auto & item : results) {
        MLOG(INFO) << "uniform, item: " << item;
    }

    EXPECT_EQ(1, 1);
}

TEST(NormalTest, normal) {
    float start = 0.0;
    float end = 10.0;

    float result = Random::instance().normal<float>(start, end);
    MLOG(INFO) << "normal, result: " << result;

    int num = 10;
    std::vector<float> results = Random::instance().normal<float>(start, end, num);
    for (auto & item : results) {
        MLOG(INFO) << "normal, item: " << item;
    }

    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
