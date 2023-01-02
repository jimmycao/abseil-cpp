struct Item {
    std::string name;
};

typedef std::function<void(std::shared_ptr<Item>)> AsyncFunction;

class AsyncChannelThreadGroup {
public:
    AsyncChannelThreadGroup(AsyncFunction func, int thread_num) {
        _func = std::move(func);

        _tg = std::make_shared<ThreadGroup>(thread_num);

        _item_chan = make_channel<std::shared_ptr<Item>>();
        _item_chan->set_capacity(1000);
        _item_chan->set_block_size(1);
    }

    ~AsyncChannelThreadGroup() {
        _item_chan->close();
        _tg->join();
    }

    void put(std::shared_ptr<Item> item) {
        _item_chan->put(item);
    }

    void start() {
        auto thread_func = [&](int i) {
            std::shared_ptr<Item> item;
            while (_item_chan->get(item)) {
                MLOG(INFO) << "in thread i:" << i;
                this->_func(item);
            }
        };

        _tg->start(thread_func);
    }

private:
    AsyncFunction _func = nullptr;
    Channel<std::shared_ptr<Item>> _item_chan = nullptr;
    std::shared_ptr<ThreadGroup> _tg = nullptr;
};

TEST(AsyncFunctionTest, test) {
    std::function<void(std::shared_ptr<Item>)> task_func = [] (std::shared_ptr<Item> item) {
        MLOG(INFO) << "in task_func, name:" << item->name;
        std::reverse(item->name.begin(), item->name.end());
        MLOG(INFO) << "in task_func, after reverse, name:" << item->name;
    };

    int thread_num = 2;
    AsyncChannelThreadGroup async(std::move(task_func), thread_num);

    async.start();

    for (int i = 0; i < 10; i++) {
        std::shared_ptr<Item> item = std::make_shared<Item>();
        item->name = std::string("name_") + std::to_string(i);
        async.put(item);
    }

    EXPECT_EQ(1, 1);
}
