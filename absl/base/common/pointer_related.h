#pragma once

#include <memory>

namespace mlf {
/*
using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;

template<class T, class... ARGS> 
unique_ptr<T> make_unique(ARGS && ... args) { 
    return unique_ptr<T>(new T(std::forward<ARGS>(args)...)); 
}
*/

template <typename To, typename From> 
inline std::shared_ptr<To> reinterpret_pointer_cast(std::shared_ptr<From> const & ptr) noexcept {
    return std::shared_ptr<To>(ptr, reinterpret_cast<To *>(ptr.get())); 
}

} //namespace mlf
