#include <yaml-cpp/yaml.h>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/functional/hash.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>

#include "common/log.h"
#include "config.h"

namespace mlf {

#define DEFINE_NODE_CONVERTER(T) \
    bool Config::_node_2_val(const YAML::Node& node, T & out) const { \
        try { \
            out = node.as<T>(); \
            return true; \
        } catch (const std::exception& e) { \
            MLOG(WARNING) << "Error reading conf item " << _path << " : " << e.what(); \
            return false; \
        } \
        return true; \
    }

DEFINE_NODE_CONVERTER(int16_t);
DEFINE_NODE_CONVERTER(uint16_t);
DEFINE_NODE_CONVERTER(int32_t);
DEFINE_NODE_CONVERTER(uint32_t);
DEFINE_NODE_CONVERTER(int64_t);
DEFINE_NODE_CONVERTER(uint64_t);
DEFINE_NODE_CONVERTER(float);
DEFINE_NODE_CONVERTER(double);
DEFINE_NODE_CONVERTER(bool);
DEFINE_NODE_CONVERTER(std::string);
DEFINE_NODE_CONVERTER(signed char);
DEFINE_NODE_CONVERTER(unsigned char);

Config Config::_extract_key(const std::string& key) {
    Config conf = { (*_node)[key], _path + "." + key };
    return conf;
}

Config::Config() {
    _node = std::make_shared<YAML::Node>(YAML::NodeType::Undefined);
    //_node = std::make_shared<YAML::Node>();
}

Config::Config(const Config& other) : _path(other._path) {
    _node = std::make_shared<YAML::Node>();
    *_node = *(other._node);
}

Config::Config(const YAML::Node& node, const std::string& path) : _path(path) {
    _node = std::make_shared<YAML::Node>();
    *_node = node;
}

int Config::init(const std::string& conf_file) {
    try {
        MCHECK(_node != nullptr);
        YAML::Node node = YAML::LoadFile(conf_file);  
        *_node = node;
        return 0;
    } catch (YAML::Exception& e) {
        MLOG(FATAL) << "parse yaml config file " << conf_file << "failed, with error: " << e.what();
    }
    return -1;
}

Config& Config::operator=(const Config& other) {
    _path = other._path;
    (*_node).reset(*(other._node)); // don't use operator=, since it has different semantic
    return *this; 
}

bool Config::is_defined() const {
    return _node->IsDefined();
}

bool Config::is_null() const {
    return _node->IsNull();
}

bool Config::is_scalar() const {
    return _node->IsScalar();
}

bool Config::is_sequence() const {
    return _node->IsSequence();
}

bool Config::is_map() const {
    return _node->IsMap();
}

size_t Config::size() const {
    return _node->size();
}

Config Config::operator[](size_t i) const { 
    return { (*_node)[i], _path + "[" + boost::lexical_cast<std::string>(i) + "]" }; 
}

Config Config::operator[](const std::string& key) const {
    try {
        return { (*_node)[key], _path + "." + key };
    } catch (const std::exception& e) {
        MLOG(FATAL) << "path[" << _path << "]. key[" << key << "]. reason[" << e.what() << "]";
        throw;
    }
}

} //namespace mlf
