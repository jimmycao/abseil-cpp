#include <gtest/gtest.h>

#include "log.h"
#include "archive_base.h"

namespace mlf {

TEST(ArchiveBaseTest, write_buffer) {
    size_t capacity = 100;
    char* buffer = new char[capacity];
    std::function<void(char*)> delete_handler = [] (char* buffer) { delete [] buffer; };

    ArchiveBase archive;
    archive.set_write_buffer(buffer, capacity, std::move(delete_handler));

    MLOG(INFO) << "capacity: " << archive.capacity();
    EXPECT_EQ(capacity, archive.capacity());

    char to_write[] = {'1', '2', '3', '4', '5'};
    archive.write(to_write, sizeof(to_write));
    EXPECT_EQ(5, archive.length());
   
    archive.advance_cursor(5);
    EXPECT_EQ(5, archive.position());
    archive.clear();
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    int rt = RUN_ALL_TESTS();
    return rt;
}
