#include "mutable_table_view.h"

#include <godot_cpp/core/class_db.hpp>

namespace morphy
{

MutableTableView::MutableTableView()
{
}

MutableTableView::~MutableTableView()
{
}

void MutableTableView::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("set_cell", "column", "row", "value"), &MutableTableView::set_cell);
}

void MutableTableView::set_cell(int64_t p_column, int64_t p_row, const godot::Variant& p_value)
{
}

} // namespace morphy
