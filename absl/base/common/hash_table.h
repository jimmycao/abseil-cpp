#pragma once

#include <memory>
#include <iostream>

#include "common/chunk_allocator.h"

namespace mlf {
// An extensible hash table implementation. It is significantly faster than std::unordered_map
// Unlike std::unordered_map, this implementation has almost no extra cost during reallocation, 
// thanks for the adoption of partial migration, So it is particually suitable for the use in 
// parameter server, where every client waits for the slowest server node
//
// The downside of this implementation is that enumeration of elements is slower,
// because it needs to examine all buckets, including empty ones
// And it relies on the randomness of the lower bits of hash codes
// Keys should be comparable, because elements in the same bucket are sorted by their keys
//
// typedef node_ptr_t node_ptr_t;
// static const node_ptr_t NODE_PTR_END = NODE_PTR_END;

template<class KEY, class VALUE>
class HashTable {
public:
    typedef HashTable<KEY, VALUE> Self;

    struct Node : public std::pair<KEY, VALUE> {
        node_ptr_t next;
        using std::pair<KEY, VALUE>::pair;
    };

    struct const_iterator {
        const Self* table;
        node_ptr_t node;

        const_iterator() {
        }
        const_iterator(const Self* table_, node_ptr_t node_) : table(table_), node(node_) {
        }

        friend bool operator==(const const_iterator& a, const const_iterator& b) {
            return a.node == b.node;
        }
        friend bool operator!=(const const_iterator& a, const const_iterator& b) {
            return a.node != b.node;
        }

        const std::pair<KEY, VALUE>& operator*() const {
            return *(table->_alloc.addr(node));
        }
        const std::pair<KEY, VALUE>* operator->() const {
            return table->_alloc.addr(node);
        }

        void advance() {
            if (table->_alloc.addr(node)->next != NODE_PTR_END) {
                node = table->_alloc.addr(node)->next;
            } else {
                size_t hash_code = table->my_hash(table->_alloc.addr(node)->first);
                //size_t i = table->_next_nonempty_bucket(table->_which_bucket(node->hash_code) + 1);
                size_t i = table->_next_nonempty_bucket(table->_which_bucket(hash_code) + 1);
                if (i < table->_bucket_count) {
                    node = table->_access_bucket(i);
                } else {
                    node = NODE_PTR_END;
                }
            }
        }

        const_iterator& operator++() {
            advance();
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator it = *this;
            advance();
            return it;
        }
    };

    struct iterator : public const_iterator {
        using const_iterator::const_iterator;

        std::pair<KEY, VALUE>& operator*() const {
            return *(this->table->_alloc.addr(this->node));
        }

        std::pair<KEY, VALUE>* operator->() const {
            return this->table->_alloc.addr(this->node);
        }

        iterator& operator++() {
            this->advance();
            return *this;
        }
        iterator operator++(int) {
            iterator it = *this;
            this->advance();
            return it;
        }
    };

    struct const_local_iterator {
        const Self* table;
        node_ptr_t node;

        const_local_iterator() {
        }
        const_local_iterator(const Self* _table, node_ptr_t node_) : table(_table), node(node_) {
        }

        friend bool operator==(const const_local_iterator& a, const const_local_iterator& b) {
            return a.node == b.node;
        }

        friend bool operator!=(const const_local_iterator& a, const const_local_iterator& b) {
            return a.node != b.node;
        }

        const std::pair<KEY, VALUE>& operator*() const {
            return *(table->_alloc.addr(node));
        }

        const std::pair<KEY, VALUE>* operator->() const {
            return table->_alloc.addr(node);
        }

        void advance() {
            node = table->_alloc.addr(node)->next;
        }

        const_local_iterator& operator++() {
            advance();
            return *this;
        }
        const_local_iterator operator++(int) {
            const_local_iterator it = *this;
            advance();
            return it;
        }
    };

    struct local_iterator : public const_local_iterator {
        using const_local_iterator::const_local_iterator;

        std::pair<KEY, VALUE>& operator*() const {
            return *(this->table->_alloc.addr(this->node));
        }

        std::pair<KEY, VALUE>* operator->() const {
            return this->table->_alloc.addr(this->node);
        }

        local_iterator& operator++() {
            this->advance();
            return *this;
        }

        local_iterator operator++(int) {
            local_iterator it = *this;
            this->advance();
            return it;
        }
    };

    HashTable() {
        _size = 0;
        _mask = 0;

        _bucket_count = 1;
        _bucket_capacity = 1;

        _migration_cursor = 0;

        _cur_buckets.reset(new node_ptr_t[1]);
        _cur_buckets[0] = NODE_PTR_END;
    }

    ~HashTable() {
        clear();
    }

    bool empty() {
        return _size == 0;
    }

    size_t size() {
        return _size;
    }

    void clear() {
        for (size_t i = 0; i < _bucket_count; i++) {
            node_ptr_t x = _access_bucket(i);
            while (x != NODE_PTR_END) {
                node_ptr_t y = _alloc.addr(x)->next;
                _alloc.release(x);
                x = y;
                _size--;
            }
            _access_bucket(i) = NODE_PTR_END;
        }
        CHECK(_size == 0);
    }

    void dump() const {
        for (size_t i = 0; i < _bucket_count; i++) {
            node_ptr_t x = _access_bucket(i);
            while (x != NODE_PTR_END) {
                Node* n = _alloc.addr(x);
                size_t hash_code = my_hash(n->first);
                std::cout << "Bucket #" << i << ": (" << x << ", " 
                          << n->first << ", " << hash_code << ", " 
                          << _mask << ", " << _which_bucket(hash_code) << ")" << std::endl;
                CHECK(_which_bucket(hash_code) == i) << _which_bucket(hash_code) << ", " << i;
                CHECK(n->next == NODE_PTR_END || n->first < _alloc.addr(n->next)->first);
                x = n->next;
            }
        }
    }

    void print_summary() const {
        size_t nonempty_bucket_count = 0;
        for (size_t i = 0; i < _bucket_count; i++) {
            if (_access_bucket(i) != NODE_PTR_END) {
                nonempty_bucket_count++;
            }
        }
        printf("Size %lld Bucket count %lld Nonempty bucket count %lld\n", (long long)_size,
                (long long)_bucket_count, (long long)nonempty_bucket_count);
    }

    Node* addr(node_ptr_t x) {
        return _alloc.addr(x);
    }

    iterator begin() {
        size_t i = _next_nonempty_bucket(0);
        if (i < _bucket_count) {
            return {this, _access_bucket(i)};
        }
        return {this, NODE_PTR_END};
    }
    const_iterator begin() const {
        return ((Self*)this)->begin();
    }
    const_iterator cbegin() const {
        return ((Self*)this)->begin();
    }

    iterator end() {
        return {this, NODE_PTR_END};
    }
    const_iterator end() const {
        return {this, NODE_PTR_END};
    }
    const_iterator cend() const {
        return {this, NODE_PTR_END};
    }

    local_iterator begin(size_t i) {
        return {this, _access_bucket(i)};
    }
    const_local_iterator begin(size_t i) const {
        return {this, _access_bucket(i)};
    }
    const_local_iterator cbegin(size_t i) const {
        return {this,_access_bucket(i)};
    }

    local_iterator end(size_t i) {
        return {this, NODE_PTR_END};
    }
    const_local_iterator end(size_t i) const {
        return {this, NODE_PTR_END};
    }
    const_local_iterator cend(size_t i) const {
        return {this, NODE_PTR_END};
    }

    VALUE& operator[](const KEY& key) {
        node_ptr_t* node = raw_find(key);
        if (match_key(*node, key)) {
            return _alloc.addr(*node)->second;
        }
        // return raw_emplace_at(node, key, VALUE())->second;
        return raw_emplace_at(node, std::piecewise_construct, std::forward_as_tuple(key),
                std::make_tuple())->second;
    }
    VALUE& operator[](KEY && key) {
        node_ptr_t* node = raw_find(key);
        if (match_key(*node, key)) {
            return _alloc.addr(*node)->second;
        }
        // return raw_emplace_at(node, std::move(key), VALUE())->second;
        return raw_emplace_at(node, std::piecewise_construct, std::forward_as_tuple(std::move(key)),
                std::make_tuple())->second;
    }

    size_t bucket_count() const {
        return _bucket_count;
    }

    node_ptr_t* raw_find(const KEY& key) const {
        size_t hash_code = my_hash(key);
        size_t i = _which_bucket(hash_code);
        return _find_node(&_access_bucket(i), key);
    }
    iterator find(const KEY& key) {
        size_t hash_code = my_hash(key);
        size_t i = _which_bucket(hash_code);
        node_ptr_t node = _find_node(_access_bucket(i), key);
        if (match_key(node, key)) {
            return {this, node};
        }
        return {this, NODE_PTR_END};
    }
    const_iterator find(const KEY& key) const {
        return ((Self*)this)->find(key);
    }

    template<class... ARGS>
    iterator raw_emplace_at(node_ptr_t* pos, ARGS && ... args) {
        node_ptr_t node = _alloc.acquire(std::forward<ARGS>(args)...);
        _alloc.addr(node)->next = *pos;
        *pos = node;
        _after_insertion();
        return {this, node};
    }

    template<class... ARGS>
    std::pair<iterator, bool> emplace(ARGS && ... args) {
        node_ptr_t node = _alloc.acquire(std::forward<ARGS>(args)...);
        KEY key = _alloc.addr(node)->first;
        size_t hash_code = my_hash(key);
        size_t i = _which_bucket(hash_code);
        node_ptr_t* pos = _find_node(&_access_bucket(i), key);
        if (match_key(*pos, key)) {
            _alloc.release(node);
            return {{this, *pos}, false};
        }
        _alloc.addr(node)->next = *pos;
        *pos = node;
        _after_insertion();
        return {{this, node}, true};
    }

    std::pair<iterator, bool> insert(const std::pair<KEY, VALUE>& val) {
        size_t hash_code = my_hash(val.first);
        size_t i = _which_bucket(hash_code);

        node_ptr_t* pos = _find_node(&_access_bucket(i), val.first);
        if (match_key(*pos, val.first)) {
            return {{this, *pos}, false};
        }
        node_ptr_t node = _alloc.acquire(val);
        _alloc.addr(node)->next = *pos;
        *pos = node;
        _after_insertion();

        return {{this, node}, true};
    }
    std::pair<iterator, bool> insert(std::pair<KEY, VALUE> && val) {
        size_t hash_code = my_hash(val.first);
        size_t i = _which_bucket(hash_code);

        node_ptr_t* pos = _find_node(&_access_bucket(i), val.first);
        if (match_key(*pos, val.first)) {
            return {{this, *pos}, false};
        }
        node_ptr_t node = _alloc.acquire(std::move(val));
        _alloc.addr(node)->next = *pos;
        *pos = node;
        _after_insertion();

        return {{this, node}, true};
    }

    void raw_erase(node_ptr_t* node) {
        node_ptr_t x = *node;
        *node = _alloc.addr(x)->next;
        _alloc.release(x);
        _after_erasion();
    }
    size_t erase(const KEY& key) {
        size_t hash_code = my_hash(key);
        size_t i = _which_bucket(hash_code);
        node_ptr_t* x = _find_node(&_access_bucket(i), key);
        if (!match_key(*x, key)) {
            return 0;
        }
        raw_erase(x);
        return 1;
    }
    void erase(const_iterator it) {
        erase(it->first);
    }

    bool match_key(node_ptr_t node, const KEY& key) const {
        return node != NODE_PTR_END && _alloc.addr(node)->first == key;
    }

    size_t my_hash(const KEY& key) const {
        return _hash(key);
    }

    size_t mem_size() const {
        size_t total_mem_size = _alloc.mem_size();
        //for (auto iter = begin(); iter != end(); ++iter) {
        //    total_mem_size += iter->second.extra_mem_size();
        //}
        // 老桶
        total_mem_size += _bucket_capacity / 2 * sizeof(node_ptr_t);
        // 新桶中OS真正分配的物理内存
        total_mem_size += (_bucket_count - _migration_cursor) * sizeof(node_ptr_t);
        return total_mem_size;
    }

private:
    size_t _which_bucket(size_t hash_code) const {
        size_t i = hash_code & _mask;
        if (i < _bucket_count) {
            return i;
        }
        return hash_code & (_mask >> 1);
    }

    node_ptr_t& _access_bucket(size_t i) const {
        if (i >= _migration_cursor) {
            return *(node_ptr_t*)&_cur_buckets[i];
        }
        return *(node_ptr_t*)&_old_buckets[i];
    }

    size_t _next_nonempty_bucket(size_t i) const {
        while (i < _bucket_count && _access_bucket(i) == NODE_PTR_END) {
            i++;
        }
        return i;
    }

    node_ptr_t _find_node(node_ptr_t node, const KEY& key) const {
        while (node != NODE_PTR_END && _alloc.addr(node)->first < key) {
            node = _alloc.addr(node)->next;
        }
        return node;
    }

    node_ptr_t* _find_node(node_ptr_t* node, const KEY& key) const {
        while (*node != NODE_PTR_END && _alloc.addr(*node)->first < key) {
            node = &(_alloc.addr(*node)->next);
        }
        return node;
    }

    void _after_insertion() {
        _size++;
        if (_size * 2 >= _bucket_count || _bucket_count + 2 <= _bucket_capacity) {
            _grow_one();
            _migrate_one();
            _grow_one();
            _migrate_one();
        }
    }

    void _after_erasion() {
        _size--;
        if (_migration_cursor > 0) {
            _migrate_one();
        }
    }

    // all node_ptr_t* are invalidated
    void _migrate_one() {
        if (_migration_cursor > 0) {
            _migration_cursor--;
            _cur_buckets[_migration_cursor] = _old_buckets[_migration_cursor];
            if (_migration_cursor == 0) {
                _old_buckets.reset();
            }
        }
    }

    // all node_ptr_t* are invalidated
    void _grow_one() {
        if (_bucket_count >= _bucket_capacity) {
            _reserve_buckets(_bucket_capacity * 2);
        }
        if ((_bucket_count & (_bucket_count - 1)) == 0) {
            _mask = _bucket_count * 2 - 1;
        }

        size_t old_i = _bucket_count & (_mask >> 1);
        size_t new_i = _bucket_count++;
        node_ptr_t* x = &_access_bucket(old_i);
        node_ptr_t* y = &_access_bucket(new_i);
        *y = NODE_PTR_END;

        while ((*x) != NODE_PTR_END) {
            if ((my_hash(_alloc.addr(*x)->first) & _mask) == new_i) {
                *y = *x;
                *x = _alloc.addr(*x)->next;
                y = &(_alloc.addr(*y)->next);
                *y = NODE_PTR_END;
            } else {
                x = &(_alloc.addr(*x)->next);
            }
        }
    }

    void _reserve_buckets(size_t new_capacity) {
        if (new_capacity <= _bucket_capacity) {
            return;
        }
        CHECK(_migration_cursor == 0);

        _old_buckets.reset(new node_ptr_t[new_capacity]);
        _old_buckets.swap(_cur_buckets);

        _bucket_capacity = new_capacity;
        _migration_cursor = _bucket_count;
    }

private:
    size_t _size;
    // if bucket_count - 1 is 001010b, then mask is 001111b. 
    // bucket_count - 1 and mask have the same highest nonzero bit
    size_t _mask; 

    size_t _bucket_count;
    size_t _bucket_capacity; // capacity of _cur_buckets

    // if i >= _migration_cursor, access _cur_buckets, otherwise access _old_buckets
    size_t _migration_cursor; 

    std::unique_ptr<node_ptr_t[]> _old_buckets;
    std::unique_ptr<node_ptr_t[]> _cur_buckets;

    ChunkAllocator<Node> _alloc;

    std::hash<KEY> _hash;
};

} //namespace mlf
