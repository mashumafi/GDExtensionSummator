#ifndef MORPHY_UTIL_HPP
#define MORPHY_UTIL_HPP

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace morphy {

// An std compliant allocator that uses Godot's DefaultAllocator
template <class T>
struct GodotAllocator {
	using value_type = T;

	inline constexpr GodotAllocator() noexcept {
	}

	template <class U>
	inline constexpr GodotAllocator(const GodotAllocator<U> &) noexcept {
	}

	[[nodiscard]] inline constexpr T *allocate(size_t n) {
		return static_cast<T *>(godot::DefaultAllocator::alloc(n * sizeof(T)));
	}

	inline constexpr void deallocate(T *ptr, size_t) noexcept {
		godot::DefaultAllocator::free(ptr);
	}

	template <class U>
	inline constexpr bool operator==(const GodotAllocator<U> &other) const noexcept {
		return true;
	}

	template <class U>
	inline constexpr bool operator!=(const GodotAllocator<U> &other) const noexcept {
		return !this->operator==(other);
	}
};

template <class Key, class T>
using GodotPairAllocator = GodotAllocator<std::pair<Key, T>>;

// std collection types with Godot allocators
template <class Key, class T, class Compare = std::less<Key>>
using GodotMap = std::map<Key, T, Compare, GodotPairAllocator<const Key, T>>;

template <class Key, class Compare = std::less<Key>>
using GodotSet = std::set<Key, Compare, GodotAllocator<Key>>;

template <class Key, class T, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
using GodotUMap = std::unordered_map<Key, T, Hash, KeyEqual, GodotPairAllocator<const Key, T>>;

template <class Key, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>>
using GodotUSet = std::unordered_set<Key, Hash, KeyEqual, GodotAllocator<Key>>;

template <class T>
using GodotVector = std::vector<T, GodotAllocator<T>>;

using GodotString = std::basic_string<char32_t, std::char_traits<char32_t>, GodotAllocator<char32_t>>;

// Object RAII
struct ObjectDeleter {
	void operator()(godot::Object *object) const {
		memdelete(object);
	}
};

template <class T>
using UniqueObject = std::unique_ptr<T, ObjectDeleter>;

template <class T>
UniqueObject<T> make_unique() {
	return UniqueObject<T>(memnew(T), ObjectDeleter());
}

class GodotTicker {
public:
	inline GodotTicker() noexcept {
		ticks = Time->get_ticks_usec();
	}

	inline void update(const godot::String &msg) noexcept {
		uint64_t end = Time->get_ticks_usec();
		godot::UtilityFunctions::print(msg, " took ", (end - ticks) / 1000.0, " milliseconds.");
		ticks = Time->get_ticks_usec();
	}

private:
	uint64_t ticks;
	godot::Time *Time = godot::Time::get_singleton();
};

} // namespace morphy

// TODO: Why does this need to be explicitly done
template <>
struct std::hash<morphy::GodotString> {
	size_t operator()(const morphy::GodotString &s) const noexcept {
		return std::hash<std::u32string_view>{}(s.data());
	}
};

#endif // MORPHY_UTIL_HPP
