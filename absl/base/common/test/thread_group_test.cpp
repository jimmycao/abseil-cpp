#include <thread>

#include <gtest/gtest.h>

#include "log.h"
#include "thread_group.h"

namespace mlf {

TEST(ThreadGroupuTest, test) {
    std::function<void(int)> task = [] (int i) {
        MLOG(INFO) << "task: " << i << " is running...";
    };

    int thread_num = 10;
    ThreadGroup tg(thread_num);
    tg.start(task);
    tg.join();

    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
