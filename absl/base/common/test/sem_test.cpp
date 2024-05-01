#include <thread>

#include <gtest/gtest.h>

#include "log.h"
#include "sem.h"

namespace mlf {

TEST(SemTest, test) {
    Semaphore sem(0, 1);
    std::thread t1([&] () {
            sem.wait();
            MLOG(INFO) << "in t1, is waiting.. ";
            sleep(2);
            MLOG(INFO) << "in t1, is exiting.. ";
            sem.post();
            });

    std::thread t2([&] () {
            sem.wait();
            MLOG(INFO) << "in t2, is waiting.. ";
            sleep(1);
            MLOG(INFO) << "in t2, is exiting.. ";
            sem.post();
            });

    t1.join();
    t2.join();

    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
