#ifndef RESOURCEHANDLER_H
#define RESOURCEHANDLER_H
#include "Misc.h"
//#include <functional>
//#include <utility>
//#include <memory>

template <typename T>
class Resource {
public:
    using DeleterType = std::function<void(T*)>;
private:
    T* ptr = nullptr;
    DeleterType deleter;
public:
    Resource() = default;
    Resource(T* p, DeleterType d) : ptr(p), deleter(std::move(d)) {}
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
    Resource(Resource&& other) noexcept : ptr(other.ptr), deleter(std::move(other.deleter)) { other.ptr = nullptr; }
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            Reset();
            ptr = other.ptr;
            deleter = std::move(other.deleter);
            other.ptr = nullptr;
        }
        return *this;
    }
    ~Resource() { Reset();}
    void Reset(T* newPtr = nullptr) {
        if (ptr && deleter) {
            deleter(ptr);
        }
        ptr = newPtr;
    }

    [[nodiscard]] T* Get() const noexcept { return ptr; }
    [[nodiscard]] T& operator*() const noexcept { return *ptr; }
    [[nodiscard]] T* operator->() const noexcept { return ptr; }
    explicit operator bool() const noexcept { return ptr != nullptr; }
	operator T* () const noexcept { return ptr; }
};
#endif