#include <views/column_accessor.hpp>

namespace morphy {

void ColumnAccessor::set_view(ITableView *p_view) {
	view = p_view;
}

void ColumnAccessor::set_current_column(uint64_t p_current_column) {
	current_column = p_current_column;
}

void ColumnAccessor::set_dependency_tracker(DependencyTracker *p_dependency_tracker) {
	dependency_tracker = p_dependency_tracker;
}

const godot::Variant &ColumnAccessor::cell(uint64_t cell) const {
	static const godot::Variant nil;
	ERR_FAIL_COND_V(!view, nil);

	if (dependency_tracker) {
		dependency_tracker->add_dependency(current_column, cell);
	}

	return view->get_cell(current_column, cell);
}

void ColumnAccessor::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("cell", "cell"), &ColumnAccessor::cell);
}

} // namespace morphy
