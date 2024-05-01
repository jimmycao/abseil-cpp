#pragma once

namespace mlf {

class Object {
};

/**
 * in boost, 'boost::noncopyable' is provided.
 *
 * #include <boost/core/noncopyable.hpp>
 */
class NoncopyableObject : public Object {
public:
    NoncopyableObject() = default;
    NoncopyableObject(const NoncopyableObject&) = delete;
    NoncopyableObject& operator=(const NoncopyableObject&) = delete;
};

class VirtualObject : public NoncopyableObject {
public:
    virtual ~VirtualObject() = default;
};

}
