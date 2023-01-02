#include <deque>
#include <mutex>
#include <memory>
#include <vector>
#include <algorithm>
#include <condition_variable>

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

template<class T>
class ChannelObject {
public:
    explicit ChannelObject(size_t capacity) {
        _capacity = std::min(_max_capacity(), capacity);
    }
    ChannelObject() = default;
    ChannelObject(const ChannelObject&) = delete;
    ChannelObject& operator=(const ChannelObject&) = delete;

    size_t capacity() {
        return _capacity;
    }

    void set_capacity(size_t x) {
        std::lock_guard<std::mutex> lock(_mutex);
        _capacity = std::min(_max_capacity(), x);
        _notify();
    }

    void set_max_capacity() {
        std::lock_guard<std::mutex> lock(_mutex);
        _capacity = _max_capacity();
        _notify();
    }

    size_t block_size() {
        return _block_size;
    }

    void set_block_size(size_t x) {
        MCHECK(x >= 1);
        std::lock_guard<std::mutex> lock(_mutex);
        _block_size = x;
    }

    template<class TT>
    void inherit_from(const std::shared_ptr<ChannelObject<TT>>& other) {
        std::lock_guard<std::mutex> lock(_mutex);
        _capacity = other->capacity();
        _block_size = other->block_size();
    }

    bool closed() {
        return _closed;
    }

    void open() {
        std::lock_guard<std::mutex> lock(_mutex);
        _closed = false;
        _notify();
    }

    void close() {
        std::lock_guard<std::mutex> lock(_mutex);
        _closed = true;
        _notify();
    }

    size_t size() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _data.size();
    }
 
    bool empty() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _data.empty();
    }

    // ================ put / get =====================
    // blocking operation
    bool put(T && val) {
        return write_move(1, &val) != 0;
    }

    // blocking operation
    bool put(const T& val) {
        return write(1, &val) != 0;
    }

    bool get(T& val) {
        return read(1, &val) != 0;
    }

    bool try_get(T& val) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_data.size() == 0) {
            return false;
        } else {
            size_t finished = _read(1, &val, lock);
            _notify();
        }
        return true;
    }
 
    // closed & not enough -> -1
    // not enough -> 0
    // enough -> 1
    int try_get(size_t n, T* p) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_closed && _data.size() < n) {
            return -1;
        } else if (_data.size() < n) {
            return 0;
        }

        size_t finished = _read(n, p, lock);
        _notify();
        MCHECK(finished == n);
        return 1;
    }
  
    
    // ============== write/read from/to array with address p
    // blocking operation
    // returns value less than n if the channel is closed
    size_t write(size_t n, const T* p) {
        if (n == 0) {
            return 0;
        }

        std::unique_lock<std::mutex> lock(_mutex);
        size_t finished = _write(n, p, lock);
        _notify();
        return finished;
    }

    // write_move() will clear original contents of p
    size_t write_move(size_t n, T* p) {
        if (n == 0) {
            return 0;
        }

        std::unique_lock<std::mutex> lock(_mutex);
        size_t finished = _write_move(n, p, lock);
        _notify();
        return finished;
    }

    //blocking operation
    // returns 0 if the channel is closed and empty
    size_t read(size_t n, T* p) {
        if (n == 0) {
            return 0;
        }

        std::unique_lock<std::mutex> lock(_mutex);
        size_t finished = _read(n, p, lock);
        _notify();
        return finished;
    }

    // ============ write/read from/to vector with block ===============
    size_t write(const std::vector<T>& p) {
        return write(p.size(), &p[0]);
    }

    size_t write(std::vector<T> && p) {
        return write_move(p.size(), &p[0]);
    }

    // read data with block_size
    size_t read(std::vector<T>& p) {
        p.resize(_block_size);
        size_t finished = read(p.size(), &p[0]);
        p.resize(finished);
        return finished;
    }

    size_t read_all(std::vector<T>& p) {
        p.clear();
        size_t finished = 0;
        size_t n = 0;

        do {
            n = _block_size; // _block_size may change anytime
            p.resize(finished + n);
            n = read(n, &p[finished]);
            finished += n;
        } while (n != 0);

        p.resize(finished);
        return finished;
    }

    // ============ shuffle ===============
    void shuffle() {
        std::lock_guard<std::mutex> lock(_mutex);
        std::random_shuffle(_data.begin(), _data.end());
    }

private:
    static constexpr size_t _max_capacity() {
        return std::numeric_limits<size_t>::max() / 2;
    }

    void _notify() {
        if (_read_waiters_since_empty != 0 && (!_is_empty_without_lock() || _closed)) {
            _empty_cond.notify_one();
        }

        if (_write_waiters_since_full != 0 && (!_is_full_without_lock() || _closed)) {
            _full_cond.notify_one();
        }
    }

    bool _is_empty_without_lock() {
        return _data.empty();
    }

    bool _is_full_without_lock() {
        return _data.size() >= _capacity + _reading_count;
    }

    bool _wait_for_read(std::unique_lock<std::mutex>& lock) {
        while (unlikely(_is_empty_without_lock() && !_closed)) {
            if (_write_waiters_since_full != 0) {
                _full_cond.notify_one();
            }

            _read_waiters_since_empty++;
            _empty_cond.wait(lock);
            _read_waiters_since_empty--;
        }

        return !_is_empty_without_lock();
    }

    bool _wait_for_write(std::unique_lock<std::mutex>& lock) {
        while (unlikely(_is_full_without_lock() && !_closed)) {
            if (_read_waiters_since_empty != 0) {
                _empty_cond.notify_one();
            }

            _write_waiters_since_full++;
            _full_cond.wait(lock);
            _write_waiters_since_full--;
        }

        return !_closed;
    }

    size_t _read(size_t n, T* p, std::unique_lock<std::mutex>& lock) {
        size_t finished = 0;
        MCHECK(n <= _max_capacity() - _reading_count);
        _reading_count += n;

        while (finished < n && _wait_for_read(lock)) {
            size_t m = std::min(n - finished, _data.size());

            for (size_t i = 0; i < m; i++) {
                p[finished++] = std::move(_data.front());
                _data.pop_front();
            }

            _reading_count -= m;
        }

        _reading_count -= n - finished;
        return finished;
    }

    size_t _write(size_t n, const T* p, std::unique_lock<std::mutex>& lock) {
        size_t finished = 0;

        while (finished < n && _wait_for_write(lock)) {
            size_t m = std::min(n - finished, _capacity + _reading_count - _data.size());

            for (size_t i = 0; i < m; i++) {
                _data.push_back(p[finished++]);
            }
        }

        return finished;
    }

    size_t _write_move(size_t n, T* p, std::unique_lock<std::mutex>& lock) {
        size_t finished = 0;

        while (finished < n && _wait_for_write(lock)) {
            size_t m = std::min(n - finished, _capacity + _reading_count - _data.size());

            for (size_t i = 0; i < m; i++) {
                _data.push_back(std::move(p[finished++]));
            }
        }

        return finished;
    }

private:
    size_t _capacity = _max_capacity();
    size_t _block_size = 1024;
    bool _closed = false;
    std::mutex _mutex;

    std::deque<T> _data;

    size_t _reading_count = 0;   //_readying_count collects all current read requirements

    int _read_waiters_since_empty = 0;  //for consumer(read), waiters wait because deque is empty
    int _write_waiters_since_full = 0;   //for producer(write), waiters wait because deque is full
    std::condition_variable _empty_cond;
    std::condition_variable _full_cond;
};

template<class T>
using Channel = std::shared_ptr<ChannelObject<T> >;

template<class T>
Channel<T> make_channel(size_t capacity = std::numeric_limits<size_t>::max()) {
    return std::make_shared<ChannelObject<T>>(capacity);
}

template<class T, class TT>
Channel<T> make_channel(const Channel<TT>& other) {
    MCHECK(other);
    Channel<T> chan = std::make_shared<ChannelObject<T>>();
    chan->inherit_from(other);
    return chan;
}

//GTEST
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

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    mlf::init_glog_conf();
    int rt = RUN_ALL_TESTS();
    return rt;
}
