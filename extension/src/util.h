#ifndef MORPHY_UTIL_CLASS_H
#define MORPHY_UTIL_CLASS_H

#include <godot_cpp/classes/object.hpp>

#include <limits>
#include <memory>

namespace morphy
{

constexpr auto INVALID_INDEX = std::numeric_limits<uint64_t>::max();

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
