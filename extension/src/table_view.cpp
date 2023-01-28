#include "table_view.h"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

namespace morphy
{

TableView::TableView()
{
}

TableView::~TableView()
{
}

int64_t TableView::num_columns() const
{
    return 0;
}

int64_t TableView::num_rows() const
{
    return 0;
}

const godot::Variant& TableView::get_cell(int64_t column, int64_t row) const
{
    static const godot::Variant nil;

    return nil;
}

void TableView::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("num_columns"), &TableView::num_columns);
    godot::ClassDB::bind_method(godot::D_METHOD("num_rows"), &TableView::num_rows);
    godot::ClassDB::bind_method(godot::D_METHOD("get_cell", "column", "row"), &TableView::get_cell);
}

} // namespace morphy
