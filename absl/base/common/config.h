#pragma once

#include <string>
#include <memory>
#include <stdio.h>
#include <iostream>

#include "common/log.h"

namespace YAML {
class Node;
}

namespace mlf {

// Calling operator[] to non-const YAML::Node is not thread-safe, 
// since it will create a temporary child node when the key doesn't exist
//
// Calling operator[] to const YAML::Node is thread-safe, 
// but you cannot create child node in this way
//
// operator[] of Config always call operator[] to const YAML::Node, 
// so it is thread-safe to call operator[] to Config, 
// though you cannot create child node in this way
//
// I also wonder how yaml-cpp manages its resource since it allows loops
// Looking for better c++ yaml library
class Config {
public:
    Config();

    Config(const Config& other);

    Config(const YAML::Node& node, const std::string& path);

    int init(const std::string& conf_file);

    Config& operator=(const Config& other);

    const YAML::Node& node() const {
        MCHECK(_node != nullptr);
        return *_node;
    }

    YAML::Node& node() {
        MCHECK(_node != nullptr);
        return *_node;
    }

    const std::string& path() const {
        return _path;
    }

    std::string& path() {
        return _path;
    }

    const YAML::Node& operator*() const {
        MCHECK(_node != nullptr);
        return *_node;
    }

    YAML::Node& operator*() {
        MCHECK(_node != nullptr);
        return *_node;
    }

    //const YAML::Node* operator->() const {
    //    return &(*_node);
    //}

    //YAML::Node* operator->() {
    //    return &(*_node);
    //}

    bool is_defined() const;

    bool is_null() const;

    bool is_scalar() const;

    bool is_sequence() const;

    bool is_map() const;

    size_t size() const;

    template<class T>
    bool exist(const std::string& key); 

    Config operator[](size_t i) const;

    Config operator[](const std::string& key) const;

    template<class T>
    T as() const;

private:
    Config _extract_key(const std::string& key);

    #define DECLARE_NODE_CONVERTER(T) \
        bool _node_2_val(const YAML::Node& node, T & out) const; 

    DECLARE_NODE_CONVERTER(int16_t);
    DECLARE_NODE_CONVERTER(uint16_t);
    DECLARE_NODE_CONVERTER(int32_t);
    DECLARE_NODE_CONVERTER(uint32_t);
    DECLARE_NODE_CONVERTER(int64_t);
    DECLARE_NODE_CONVERTER(uint64_t);
    DECLARE_NODE_CONVERTER(float);
    DECLARE_NODE_CONVERTER(double);
    DECLARE_NODE_CONVERTER(bool);
    DECLARE_NODE_CONVERTER(std::string);
    DECLARE_NODE_CONVERTER(signed char);
    DECLARE_NODE_CONVERTER(unsigned char);

private:
    std::shared_ptr<YAML::Node> _node = nullptr;
    std::string _path;
};

template<class T>
bool Config::exist(const std::string& key) {
    try {
        Config conf = _extract_key(key);
        T val;
        return _node_2_val(*(conf._node), val);
    } catch (...) {
        MLOG(WARNING) << "path[" << _path << "]. key[" << key << "].";
        return false;
    }
    return true;
}

template<class T>
T Config::as() const {
    T val;
    MCHECK(_node_2_val(*(_node), val));
    return val;
}

} //namespace mlf
