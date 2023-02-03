#include <util.hpp>
#include <views/basic_table.hpp>

#include <godot_cpp/core/class_db.hpp>

namespace morphy {

BasicTable::BasicTable() {
}

BasicTable::~BasicTable() {
}

void BasicTable::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("add_columns", "names"), &BasicTable::add_columns);
	godot::ClassDB::bind_method(godot::D_METHOD("add_rows", "num"), &BasicTable::add_rows);
}

uint64_t BasicTable::num_columns() const {
	return data.num_columns();
}

uint64_t BasicTable::num_rows() const {
	return data.num_rows();
}

uint64_t BasicTable::get_column_index(const godot::String &p_column_name) const {
	for (uint64_t column = 0; column < data.num_columns(); ++column) {
		if (data.get_header(column) == p_column_name) {
			return column;
		}
	}

	return INVALID_INDEX;
}

void BasicTable::add_columns(const godot::PackedStringArray &p_names) {
	ERR_FAIL_COND_MSG(p_names.size() == 0, "No columns supplied, ensure you are using a PackedStringArray.");

	GodotVector<godot::String> names;
	names.reserve(p_names.size());
	for (const godot::String &name : p_names) {
		names.push_back(name);
	}

	data.add_columns(std::move(names));
}

void BasicTable::add_rows(uint64_t p_num) {
	data.add_rows(p_num);
}

const godot::Variant &BasicTable::get_cell(uint64_t p_column, uint64_t p_row) const {
	return data.get_cell(p_column, p_row);
}

void BasicTable::set_cell(uint64_t p_column, uint64_t p_row, const godot::Variant &p_value) {
	data.set_cell(p_column, p_row, p_value);
}

} // namespace morphy
