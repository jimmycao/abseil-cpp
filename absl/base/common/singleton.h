#pragma once

#include "common/virtual_object.h"

namespace mlf {

template <typename T>
class Singleton : public VirtualObject {
public:
    virtual ~Singleton() = default;

    static T & instance() {
        static T t;
        return t;
    }
};

} //namespace mlf
