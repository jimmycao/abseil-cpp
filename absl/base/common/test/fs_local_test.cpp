#include <functional>

#include <gtest/gtest.h>

#include "log.h"
#include "line_reader.h"

#include "fs_local.h"

namespace mlf {

TEST(LocalFSTest, mkdir) {
    std::string dir_path = "./my_dir";
    LFS lfs;
    lfs.mkdir(dir_path);
    EXPECT_EQ(1, 1);
}

TEST(LocalFSTest, list) {
    std::string path = "./";
    LFS lfs;
    std::vector<std::string> results = lfs.list(path, 0);
    for (auto& item : results) {
        MLOG(INFO) << "item: " << item;
    }
    EXPECT_EQ(1, 1);
}

TEST(LocalFSTest, open) {
    std::string path = "/etc";
    std::string file = "hosts";
    std::string converter = "grep -v '^$' | grep -v '^#'";

    LFS lfs;
    std::shared_ptr<FILE> fp = lfs.open(path, file, converter,  "r");
    
    char* line = NULL;
    LineReader reader;
    while(line = reader.getline(fp.get())) {
        MLOG(INFO) << "line: " << line;
    }

    EXPECT_EQ(1, 1);
}

TEST(LocalFSTest, tail) {
    LFS lfs;
    std::string tail_3 = lfs.tail("/etc/hosts", 3);
    MLOG(INFO) << "tail_3: " << tail_3;
    EXPECT_EQ(1, 1);
}

TEST(LocalFSTest, touch) {
    LFS lfs;
    lfs.touch("./", "a_test_file");
    EXPECT_EQ(1, 1);
}

TEST(LocalFSTest, exists) {
    std::string file_path = "/etc/hosts";

    LFS lfs;
    bool is_existed = lfs.exists(file_path, "f");
    MLOG(INFO) << "is_existed: " << is_existed;
    EXPECT_EQ(true, is_existed);
}


TEST(LocalFSTest, move_remove) {
    std::string orig_file = "orig_file";
    std::string target_file = "target_file";

    LFS lfs;
    lfs.touch("./", orig_file);
    lfs.move(orig_file, target_file);
    lfs.remove(target_file);

    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
