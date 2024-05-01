#include <functional>

#include <gtest/gtest.h>

#include "log.h"
#include "scope_exit.h"

namespace mlf {

class Geeks : public ScopeExit {
public:
    explicit Geeks (std::function<void()> f) : ScopeExit(f) {
    }

    void say_hi() {
        MLOG(INFO) << "hi, everybody!";
    }
};

TEST(ScopeExitTest, scope_exit) {
    std::function<void()> f = [] () {
        MLOG(INFO) << "Bye Bye! Do remember to clean everything!";
    };

    Geeks g(f);
    g.say_hi();

    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
