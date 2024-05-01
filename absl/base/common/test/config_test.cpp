#include <gtest/gtest.h>

#include "log.h"
#include "config.h"

namespace mlf {

class ConfigTest : public testing::Test {
public:
    static void SetupTestSuite() {
        MLOG(INFO) << "SetupTestSuite...";
    }

    static void TearDownTestSuite() {
        MLOG(INFO) << "TearDownTestSuite...";
    };

    virtual void SetUp() override {
        conf.init("config.yaml");
    }

    virtual void TearDown() override {
        MLOG(INFO) << "TearDown... "; 
    }

    Config conf;
};

TEST_F(ConfigTest, exist) {
    MLOG(INFO) << "thread_num exist: " << conf.exist<std::string>("thread_num");
    EXPECT_EQ(true, conf.exist<std::string>("thread_num"));
    EXPECT_EQ(false, conf.exist<std::string>("thread_num_not_exist"));
}

TEST_F(ConfigTest, as_int) {
    MLOG(INFO) << "thread_num: " << conf["thread_num"].as<int>();
    EXPECT_EQ(10, conf["thread_num"].as<int>());
}

TEST_F(ConfigTest, as_bool) {
    MLOG(INFO) << "is_decay: " << conf["is_decay"].as<bool>();
    EXPECT_EQ(true, conf["is_decay"].as<bool>());
}

TEST_F(ConfigTest, as_float) {
    MLOG(INFO) << "decay_factor: " << conf["decay_factor"].as<float>();
    EXPECT_FLOAT_EQ(0.999, conf["decay_factor"].as<float>());
}

TEST_F(ConfigTest, as_list) {
    EXPECT_EQ(1, conf["steps"][0].as<int>());
    EXPECT_EQ(5, conf["steps"][1].as<int>());
    EXPECT_EQ(9, conf["steps"][2].as<int>());
}

TEST_F(ConfigTest, as_string) {
    EXPECT_EQ("hdfs://hostname:54310", conf["hdfs_uri"].as<std::string>());
    EXPECT_EQ("user,passwd", conf["hdfs_ugi"].as<std::string>());
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    int rt = RUN_ALL_TESTS();
    return rt;
}
