#ifndef MORPHY_VIEWS_TABLE_DATA_HPP
#define MORPHY_VIEWS_TABLE_DATA_HPP

#include <util.hpp>

#include <godot_cpp/templates/vector.hpp>

namespace morphy {

struct EmptyMeta {
};

template <typename M>
struct Cell {
	M meta;
	godot::Variant value;
};

template <typename H, typename M = EmptyMeta>
class TableData {
public:
	using Cell = struct Cell<M>;
	using Row = GodotVector<Cell>;

	uint64_t num_columns() const {
		return headers.size();
	}

	uint64_t num_rows() const {
		return rows.size();
	}

	void add_columns(GodotVector<H> &&p_headers) {
		headers.reserve(headers.size() + p_headers.size());
		for (const H &header : p_headers) {
			headers.push_back(std::move(header));
		}

		for (Row &row : rows) {
			row.resize(headers.size());
		}
	}

	void add_columns(const GodotVector<H> &p_headers) {
		headers.reserve(headers.size() + p_headers.size());
		for (const H &header : p_headers) {
			headers.push_back(header);
		}

		for (Row &row : rows) {
			row.resize(headers.size());
		}
	}

	void add_rows(uint64_t num) {
		uint64_t initial_size = rows.size();
		rows.resize(rows.size() + num);

		for (uint64_t row = initial_size; row < rows.size(); ++row) {
			rows[row].resize(headers.size());
		}
	}

	void set_cell(uint64_t column, uint64_t row, const godot::Variant &value) {
		ERR_FAIL_COND(rows.size() <= row);
		ERR_FAIL_COND(rows[row].size() <= column);

		rows[row][column].value = value;
	}

	const H &get_header(uint64_t column) const {
		static H nil;
		ERR_FAIL_COND_V(headers.size() <= column, nil);

		return headers[column];
	}

	H &get_header(uint64_t column) {
		static H nil;
		ERR_FAIL_COND_V(headers.size() <= column, nil);

		return headers[column];
	}

	const godot::Variant &get_cell(uint64_t column, uint64_t row) const {
		static const godot::Variant nil;
		ERR_FAIL_COND_V(rows.size() <= row, nil);
		ERR_FAIL_COND_V(rows[row].size() <= column, nil);

		return rows[row][column].value;
	}

	M *get_cell_meta(uint64_t column, uint64_t row) {
		ERR_FAIL_COND_V(rows.size() <= row, nullptr);
		ERR_FAIL_COND_V(rows[row].size() <= column, nullptr);

		return &rows[row][column].meta;
	}

private:
	GodotVector<H> headers;
	GodotVector<Row> rows;
};

} // namespace morphy

#endif // MORPHY_VIEWS_TABLE_DATA_HPP
