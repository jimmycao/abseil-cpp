#include <thread>

#include <gtest/gtest.h>

#include "log.h"
#include "channel.h"

namespace mlf {

TEST(ChannelTest, put_get) {
    auto test_chan = make_channel<std::string>();
    EXPECT_EQ(0, test_chan->size());

    std::thread t1([&] () { test_chan->put("t1_input"); });
    std::thread t2([&] () { test_chan->put("t2_input"); });
    t1.join();
    t2.join();
    
    EXPECT_EQ(2, test_chan->size());
    
    //since t1/t2 are thread, we don't which is the front
    std::string tmp;
    test_chan->get(tmp);
    EXPECT_TRUE("t1_input" == tmp || "t2_input" == tmp);
    test_chan->get(tmp);
    EXPECT_TRUE("t1_input" == tmp || "t2_input" == tmp);

    EXPECT_EQ(0, test_chan->size());
}

TEST(ChannelTest, write_read_with_block) {
    auto test_chan = make_channel<std::string>();

    std::vector<std::string> to_write;
    for (int i = 0; i < 1500; ++i) { 
        to_write.push_back("str_" + std::to_string(i));
    }

    test_chan->write(to_write);
    EXPECT_EQ(1500, test_chan->size());

    std::vector<std::string> to_read;

    test_chan->read(to_read);
    MLOG(INFO) << "to_read size: " << to_read.size(); 
    EXPECT_EQ(1024, to_read.size());
    /*
    for (auto& item : to_read) {
        MLOG(INFO) << "to_read, item: " << item;
    }
    */

    //read is blocking operation, by default, it will wait until block_size 
    //or the channel is closed
    test_chan->close();
    test_chan->read(to_read);
    MLOG(INFO) << "to_read size: " << to_read.size();
    /*
    for (auto& item : to_read) {
        MLOG(INFO) << "to_read, item: " << item;
    }
    */
    EXPECT_EQ(476, to_read.size());
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    int rt = RUN_ALL_TESTS();
    return rt;
}
