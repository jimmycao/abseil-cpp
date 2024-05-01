#include <mutex>
#include <thread>
#include <condition_variable>

#include <gtest/gtest.h>

#include "log.h"
#include "thread_group.h"

namespace mlf {

class AsyncThreadGroupByCondition {
public:
    AsyncThreadGroupByCondition(int thread_num) {
        _tg = std::make_shared<ThreadGroup>(thread_num);
    }

    ~AsyncThreadGroupByCondition() {
        _tg->join();
    }

    void notify_ready() {
        std::unique_lock<std::mutex> lock(_mutex);
        _is_ready = true;
        _cv.notify_all();
    }

    void start() {
        auto thread_func = [&](int i) {
            std::unique_lock<std::mutex> lock(_mutex);

            while (!_is_ready) _cv.wait(lock);

            std::string result = "" + std::to_string(i);
            for (auto& item : _items) {
                result += "_" + item;
            }

            MLOG(INFO) << "thread_id:"  << i << ", result:" << result;
        };

        _tg->start(thread_func);
    }

    void fill_data(std::vector<std::string>& datas) {
        for (auto data : datas) {
            _items.emplace_back(std::move(data));
        }
    }

private:
    bool _is_ready = false;
    std::mutex _mutex;
    std::condition_variable _cv;

    std::vector<std::string> _items;

    std::shared_ptr<ThreadGroup> _tg = nullptr;
};

TEST(AsyncFunctionTest, test) {
    int thread_num = 2;
    AsyncThreadGroupByCondition async(thread_num);

    async.start();
    MLOG(INFO) << "--------------AsyncThreadGroupByCondition---------";

    std::vector<std::string> cities {"shanghai", "beijing", "xian"};
    async.fill_data(cities);
    async.notify_ready();

    EXPECT_EQ(1, 1);
}

} //namespace mlf

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    return RUN_ALL_TESTS();
}
