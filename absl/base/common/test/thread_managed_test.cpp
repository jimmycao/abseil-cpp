#include <thread>

#include <gtest/gtest.h>

#include "log.h"
#include "thread_managed.h"

namespace mlf {

TEST(ThreadManagedTest, test) {
    std::function<void()> task = [] () {
        MLOG(INFO) << "task is running...";
    };

    ManagedThread mt;
    mt.start(std::move(task));
    MLOG(INFO) << "active: " << mt.active();
    mt.join();

    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
