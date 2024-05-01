#include <glog/logging.h>
#include <gtest/gtest.h>

#include "log.h"
#include "archive_text.h"
#include "line_reader.h"

namespace mlf {

class LineReaderTest : public testing::Test {
public:
    static void SetupTestSuite() {
        MLOG(INFO) << "SetupTestSuite...";
    }

    static void TearDownTestSuite() {
        MLOG(INFO) << "TearDownTestSuite...";
    }

    virtual void SetUp() override {
        fp = ::fopen("/etc/hosts", "r");
        CHECK(fp);
        MLOG(INFO) << "fopen finished";
    }

    virtual void TearDown() override {
        PCHECK(0 == fclose(fp));
        MLOG(INFO) << "fclose finished";
    }

    FILE* fp;
};

TEST_F(LineReaderTest, getdelim) {
    LineReader reader;
    char* content = reader.getdelim(fp, '\n');
    MLOG(INFO) << "after getdelim, content: " << content;
    EXPECT_EQ(1, 1);
}

TEST_F(LineReaderTest, getline) {
    LineReader reader;
    char* content = reader.getline(fp);
    MLOG(INFO) << "after geline, content: " << content;
    EXPECT_EQ(1, 1);
}

TEST_F(LineReaderTest, get) {
    LineReader reader;
    char* content = reader.getline(fp);
    MLOG(INFO) << "after geline, content: " << content;
    
    TextArchive oar = reader.get<TextArchive>();
    std::string result;
    oar >> result;
    MLOG(INFO) << "result: " << result;

    MLOG(INFO) << "length: " << reader.length();

    reader.reset();
    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
