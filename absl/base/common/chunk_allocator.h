#pragma once

#include <limits> 
#include "glog/logging.h"

namespace mlf {

//Fast allocate/de-allocate T from Chunk

typedef uint32_t node_ptr_t;

const node_ptr_t NODE_PTR_END = std::numeric_limits<uint32_t>::max();

template<class T>
class ChunkAllocator {
public:
    explicit ChunkAllocator(size_t offbits = 8) {
        CHECK(offbits > 1 && offbits < 16);
        CHECK(sizeof(Node) == std::max(sizeof(void*), sizeof(T)));
        _offbits = offbits;
        _chunk_size = (1 << _offbits);
        _tail_node_idx = NODE_PTR_END ;
        _acq_node_counter = 0;
    }

    ~ChunkAllocator() {
        CHECK(_acq_node_counter == 0);
        for (Chunk* x : _chunks) {
            free(x);
            x = NULL;
        }
    }

    //acquire a node from Chunk 
    template<class... ARGS>
    node_ptr_t acquire(ARGS && ... args) {
        if (_tail_node_idx == NODE_PTR_END) {
            _create_new_chunk();
        }
        //T* x = (T*)(void*)_tail_node_idx;
        T* x = addr(_tail_node_idx);
        node_ptr_t acq_node = _tail_node_idx;
        _tail_node_idx = ((Node*)addr(_tail_node_idx))->next;
        new(x) T(std::forward<ARGS>(args)...);
        _acq_node_counter++;
        return acq_node;
    }

    //return a node to Chunk
    void release(node_ptr_t x) {
        T* t = addr(x);
        t->~T();
        Node* node = (Node*)(void*)t;
        node->next = _tail_node_idx;
        _tail_node_idx = x;
        _acq_node_counter--;
    }

    T* addr(node_ptr_t p_node) {
        //uint32_t chunk_index = (p_node >> _offbits);
        //uint32_t node_index = (p_node & ((1 << _offbits) - 1));
        //CHECK(chunk_index < _chunks.size() && node_index < _chunk_size) << p_node;
        return (T*)(&(_chunks[(p_node >> _offbits)]->nodes[p_node & ((1 << _offbits) - 1)]));
    }

    T* addr(node_ptr_t p_node) const {
        CHECK((p_node >> _offbits) < _chunks.size() && (p_node & ((1 << _offbits) - 1)) < _chunk_size);
        return (T*)(&(_chunks[(p_node >> _offbits)]->nodes[p_node & ((1 << _offbits) - 1)]));
    }

    size_t mem_size() const {
        return sizeof(Node) * _chunk_size * _chunks.size();
    }

private:
    //a Node of T can be placed in data with sizeof(T)  
    struct alignas(T) Node {
        union {
            node_ptr_t next;
            char data[sizeof(T)];
        };
    };

    // a Chunk with id has a number of Nodes
    struct Chunk {
        uint32_t id;
        Node nodes[];
    };
    
     //Actually, Chunk is to simulated list, it consists of a number of Node with continuous 
     //memeory. Between Nodes, idx is regarded as address and pointer one by one
    void _create_new_chunk() {
        Chunk* chunk;

        //malloc memory for a Chunk with _chunk_size of Node; assign id for this Chunk 
        //push this Chunk to _chunks (list)
        CHECK(chunk = (Chunk*)malloc(sizeof(uint32_t) + sizeof(Node) * _chunk_size));
        size_t cur_size = _chunks.size();
        chunk->id = cur_size;
        _chunks.push_back(chunk); //push a chunk's pointer into vector (_chunks) 

        // _chunk_size of Node in a Chunk.
        for (size_t i = 0; i < _chunk_size; i++) {
            int64_t base = (cur_size << _offbits);
            int64_t index = base + i;
            CHECK(index >= 0 && index < std::numeric_limits<uint32_t>::max());
            node_ptr_t new_node_idx = (node_ptr_t)(index);
            chunk->nodes[i].next = _tail_node_idx;
            _tail_node_idx = new_node_idx;
        }
    }

private:
    size_t _chunk_size; // how many Nodes in one chunk
    uint32_t _offbits;
    std::vector<Chunk*> _chunks; // a list
    node_ptr_t _tail_node_idx; 
    size_t _acq_node_counter; // how many elements are acquired
};

}
