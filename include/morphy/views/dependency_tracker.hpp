#ifndef MORPHY_VIEWS_DEPENDENCY_TRACKER_HPP
#define MORPHY_VIEWS_DEPENDENCY_TRACKER_HPP

#include <util.hpp>

#include <optional>

namespace morphy {

struct Dependency {
	uint64_t column = 0UL;
	// Allow depending on an entire column
	std::optional<uint64_t> row = std::nullopt;

	bool operator<(const Dependency &other) const {
		if (column < other.column) {
			return true;
		} else if (column > other.column) {
			return false;
		}

		if (!row && other.row) {
			return true;
		} else if (row && !other.row) {
			return false;
		}

		if (row.value() < other.row.value()) {
			return true;
		} else if (row.value() > other.row.value()) {
			return false;
		}

		return false; // They are equal
	}

	bool operator==(const Dependency &other) const {
		return column == other.column && row == other.row;
	}

	bool operator!=(const Dependency &other) const {
		return !this->operator==(other);
	}
};

struct DependencyOwner {
	uint64_t column = 0;
	uint64_t row = 0;

	bool operator<(const DependencyOwner &other) const {
		if (column < other.column) {
			return true;
		} else if (column > other.column) {
			return false;
		}

		if (row < other.row) {
			return true;
		} else if (row > other.row) {
			return false;
		}

		return false; // They are equal
	}
};

using Dependencies = GodotSet<Dependency>;
using DependencyOwners = GodotSet<DependencyOwner>;
using Requirements = GodotMap<Dependency, DependencyOwners>;

class DependencyTracker {
public:
	// Adds a dependency and returns true if it succeeds, otherwise there could be a cycle
	bool add_dependency(uint64_t column, std::optional<uint64_t> row) {
		return dependencies.insert({ column, row }).second;
	}

	const Dependencies &get_dependencies() const {
		return dependencies;
	}

private:
	Dependencies dependencies;
};

} //namespace morphy

#endif
