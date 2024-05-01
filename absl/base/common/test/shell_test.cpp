#include <gtest/gtest.h>

#include "log.h"
#include "line_reader.h"

#include "shell.h"

namespace mlf {

TEST(ShellTest, fopen) {
    std::shared_ptr<FILE> fp = Shell().fopen("/etc/hosts", "r");

    LineReader reader;
    char* line = NULL;
    while (line = reader.getline(fp.get())) {
        MLOG(INFO) << "line: " << line;
    }

    EXPECT_EQ(1, 1);
}

TEST(ShellTest, popen) {
    std::string cmd = "ls -l /etc/host*";
    std::shared_ptr<FILE> fp = Shell().popen(cmd, "r");

    LineReader reader;
    char* line = NULL;
    while (line = reader.getline(fp.get())) {
        MLOG(INFO) << "line: " << line;
    }

    EXPECT_EQ(1, 1);
}

TEST(ShellTest, execute) {
    std::string cmd = "ls -l /etc/host*";
    bool ret_flag = true;
    std::string out = Shell().execute(cmd, ret_flag);
    MLOG(INFO) << "out: " << out;

    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
