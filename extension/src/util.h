#ifndef MORPHY_UTIL_CLASS_H
#define MORPHY_UTIL_CLASS_H

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/memory.hpp>

#include <limits>
#include <map>
#include <memory>
#include <set>

namespace morphy
{

constexpr auto INVALID_INDEX = std::numeric_limits<uint64_t>::max();

template<typename T>
struct pointer_traits {
  using reference = T&;
  using const_reference = const T&;
};

// Avoid declaring a reference to void with an empty specialization
template<>
struct pointer_traits<void> {
};

// An std compliant allocator that uses Godot's DefaultAllocator
template<class T = void>
struct GodotAllocator : public pointer_traits<T>
{
    using Allocator = godot::DefaultAllocator;
    using value_type = T;
    using size_type = size_t;
    using pointer = T*;
    using const_pointer = const T*;
    using difference_type = typename std::pointer_traits<pointer>::difference_type;

    GodotAllocator () noexcept {}
    ~GodotAllocator () noexcept {}
 
    template<class U>
    constexpr GodotAllocator (const GodotAllocator<U>&) noexcept {}
 
    T* allocate(size_t n)
    {
        return static_cast<T*>(Allocator::alloc(n * sizeof(T)));
    }
 
    void deallocate(T* ptr, size_t) noexcept
    {
        Allocator::free(static_cast<void*>(ptr));
    }
    
    template<typename U>
    struct rebind {
        typedef GodotAllocator<U> other;
    };
};

template<typename T, typename U>
constexpr bool operator==(const GodotAllocator<T>&, const GodotAllocator<U>&) noexcept
{
    return true;
}

template<typename T, typename U>
constexpr bool operator!=(const GodotAllocator<T>&, const GodotAllocator<U>&) noexcept
{
    return true;
}

// std collection types with Godot allocators
template<typename Key, typename Compare = std::less<Key>>
using GodotSet = std::set<Key, Compare, GodotAllocator<Key>>;

template<typename Key, typename T>
using GodotPairAllocator = GodotAllocator<std::pair<Key, T>>;

template<typename Key, typename T, typename Compare = std::less<Key>>
using GodotMap = std::map<Key, T, Compare, GodotPairAllocator<const Key, T>>;

template<typename T>
using GodotVector = std::vector<T, GodotAllocator<T>>;

struct ObjectDeleter
{
    void operator()(godot::Object* object) const
    {
        memdelete(object);
    }
};

template<typename T>
using UniqueObject = std::unique_ptr<T, ObjectDeleter>;

template<typename T>
UniqueObject<T> make_unique()
{
    return UniqueObject<T>(memnew(T), ObjectDeleter());
}

} // namespace morphy

#endif // MORPHY_UTIL_CLASS_H
