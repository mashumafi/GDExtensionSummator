#ifndef UTIL_HPP
#define UTIL_HPP

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/string.hpp>

#include <limits>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace morphy {

constexpr auto INVALID_INDEX = std::numeric_limits<uint64_t>::max();

// An std compliant allocator that uses Godot's DefaultAllocator
template <class T>
struct GodotAllocator {
	using Allocator = godot::DefaultAllocator;

	using value_type = T;
	using pointer = T *;
	using const_pointer = const T *;
	using reference = T &;
	using const_reference = const T &;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;
	using propagate_on_container_move_assignment = std::true_type;

	template <class U>
	struct rebind {
		typedef GodotAllocator<U> other;
	};

	using is_always_equal = std::true_type;

	inline GodotAllocator() noexcept {
	}

	inline ~GodotAllocator() {
	}

	inline GodotAllocator(const GodotAllocator &) noexcept {
	}

	template <class U>
	inline GodotAllocator(const GodotAllocator<U> &) noexcept {
	}

	inline pointer address(reference r) const noexcept {
		return &r;
	}

	inline const_pointer address(const_reference r) const noexcept {
		return &r;
	}

	inline pointer allocate(size_t n) {
		return static_cast<T *>(Allocator::alloc(n * sizeof(T)));
	}

	inline void deallocate(pointer ptr, size_t) {
		Allocator::free(static_cast<void *>(ptr));
	}

	inline size_type max_size() const noexcept {
		return std::numeric_limits<size_type>::max() / sizeof(T);
	}

	template <class U, class... Args>
	inline void construct(U *p, Args &&...args) {
		new ((void *)p) U(std::forward<Args>(args)...);
	}

	template <class U>
	inline void destroy(U *p) {
		p->~U();
	}
};

template <class T1, class T2>
bool operator==(const GodotAllocator<T1> &lhs, const GodotAllocator<T2> &rhs) noexcept {
	return &lhs == &rhs;
}

template <class T1, class T2>
bool operator!=(const GodotAllocator<T1> &lhs, const GodotAllocator<T2> &rhs) noexcept {
	return !(lhs == rhs);
}

// std collection types with Godot allocators
template <typename Key, typename Compare = std::less<Key>>
using GodotSet = std::set<Key, Compare, GodotAllocator<Key>>;

template <typename Key, typename T>
using GodotPairAllocator = GodotAllocator<std::pair<Key, T>>;

template <typename Key, typename T, typename Compare = std::less<Key>>
using GodotMap = std::map<Key, T, Compare, GodotPairAllocator<const Key, T>>;

template <typename Key, typename T, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
using GodotUMap = std::unordered_map<Key, T, Hash, KeyEqual, GodotPairAllocator<const Key, T>>;

template <typename T>
using GodotVector = std::vector<T, GodotAllocator<T>>;

// TODO: replace std::allocator with GodotAllocator
using GodotString = std::basic_string<char32_t, std::char_traits<char32_t>, std::allocator<char32_t>>;

struct ObjectDeleter {
	void operator()(godot::Object *object) const {
		memdelete(object);
	}
};

template <typename T>
using UniqueObject = std::unique_ptr<T, ObjectDeleter>;

template <typename T>
UniqueObject<T> make_unique() {
	return UniqueObject<T>(memnew(T), ObjectDeleter());
}

} // namespace morphy

#endif // UTIL_HPP
