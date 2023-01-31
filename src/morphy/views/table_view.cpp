#include <util.hpp>
#include <views/table_view.hpp>

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

namespace morphy {

TableView::TableView() {
}

TableView::~TableView() {
}

uint64_t TableView::num_columns() const {
	return 0;
}

uint64_t TableView::num_rows() const {
	return 0;
}

uint64_t TableView::get_column_index(const godot::String &p_column_name) const {
	return INVALID_INDEX;
}

const godot::Variant &TableView::get_cell(uint64_t column, uint64_t row) const {
	static const godot::Variant nil;

	return nil;
}

void TableView::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("num_columns"), &TableView::num_columns);
	godot::ClassDB::bind_method(godot::D_METHOD("num_rows"), &TableView::num_rows);
	godot::ClassDB::bind_method(godot::D_METHOD("get_column_index"), &TableView::get_column_index);
	godot::ClassDB::bind_method(godot::D_METHOD("get_cell", "column", "row"), &TableView::get_cell);
}

} // namespace morphy
