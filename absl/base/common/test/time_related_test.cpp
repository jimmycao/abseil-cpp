#include <gtest/gtest.h>

#include "log.h"
#include "time_related.h"

namespace mlf {

TEST(StringToolsTest, format_string) {
    double walltime = current_realtime();
    MLOG(INFO) << "walltime: " << walltime;

    uint64_t walltime_s = current_realtime_s();
    MLOG(INFO) << "walltime_s: " << walltime_s;
    
    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
