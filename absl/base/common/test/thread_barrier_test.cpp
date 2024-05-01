#include <thread>

#include <gtest/gtest.h>

#include "log.h"
#include "thread_barrier.h"

namespace mlf {

TEST(ThreadBarrierTest, thread_wait) {
    ThreadBarrier barrier(1);

    barrier.reset(2);

    std::thread t1([&] () { 
            sleep(1); 
            barrier.wait(); 
            sleep(2);  
            MLOG(INFO) << "in t1";
            });
    std::thread t2([&] () {
            sleep(1);
            barrier.wait();
            sleep(2);
            MLOG(INFO) << "in t2";
            });

    t1.join();
    t2.join();

    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    int rt = RUN_ALL_TESTS();
    return rt;
}
