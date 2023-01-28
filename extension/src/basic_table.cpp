#include "basic_table.h"

#include <godot_cpp/core/class_db.hpp>

namespace morphy
{

BasicTable::BasicTable()
{
}

BasicTable::~BasicTable()
{
}

void BasicTable::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("add_columns", "names"), &BasicTable::add_columns);
    godot::ClassDB::bind_method(godot::D_METHOD("add_rows", "num"), &BasicTable::add_rows);
}

int64_t BasicTable::num_columns() const
{
    return data.num_columns();
}

int64_t BasicTable::num_rows() const
{
    return data.num_rows();
}

void BasicTable::add_columns(const godot::PackedStringArray& p_names)
{
    ERR_FAIL_COND_MSG(p_names.size() == 0, "No columns supplied, ensure you are using a PackedStringArray.");

    std::vector<godot::String> names;
    names.reserve(p_names.size());
    for (const godot::String& name : p_names)
        names.push_back(name);

    data.add_columns(std::move(names));
}

void BasicTable::add_rows(int64_t p_num)
{
    data.add_rows(p_num);
}

const godot::Variant& BasicTable::get_cell(int64_t p_column, int64_t p_row) const
{
    return data.get_cell(p_column, p_row);
}

void BasicTable::set_cell(int64_t p_column, int64_t p_row, const godot::Variant& p_value)
{
    data.set_cell(p_column, p_row, p_value);
}

} // namespace morphy
