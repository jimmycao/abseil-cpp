#pragma once

#include <boost/core/noncopyable.hpp>

namespace mlf {

class ScopeExit : public boost::noncopyable {
public: 
    explicit ScopeExit(std::function<void ()> exit_func) : _f(std::move(exit_func)) { 
    } 
    
    ~ScopeExit() { 
        _f(); 
    } 

private: 
    std::function<void ()> _f;
};

} //namespace mlf
