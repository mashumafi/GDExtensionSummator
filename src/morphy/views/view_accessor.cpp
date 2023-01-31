#include <views/view_accessor.hpp>

namespace morphy {

ViewAccessor::ViewAccessor() {
}

void ViewAccessor::set_view(ITableView *p_view) {
	view = p_view;
}

void ViewAccessor::set_current_row(uint64_t p_current_row) {
	current_row = p_current_row;
}

uint64_t ViewAccessor::row() const {
	return current_row;
}

void ViewAccessor::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("row"), &ViewAccessor::row);
	godot::ClassDB::bind_method(godot::D_METHOD("get_column"), &ViewAccessor::get_column);
	godot::ClassDB::bind_method(godot::D_METHOD("column"), &ViewAccessor::get_column);
}

ColumnAccessor *ViewAccessor::get_column(const godot::String &column_name) {
	static ColumnAccessor *const nil = nullptr;

	ERR_FAIL_COND_V_MSG(!view, nil, "Invalid view");

	uint64_t column_index = view->get_column_index(column_name);
	ERR_FAIL_COND_V_MSG(column_index == INVALID_INDEX, nil, "Column not found");

	auto itr = column_cache.insert_or_assign(column_name.ptr(), make_unique<ColumnAccessor>());
	ColumnAccessor *const column_accessor = itr.first->second.get();
	column_accessor->set_view(view);
	column_accessor->set_current_column(column_index);

	return column_accessor;
}

} // namespace morphy
