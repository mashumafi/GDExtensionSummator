#ifndef MORPHY_BASIC_TABLE_CLASS_H
#define MORPHY_BASIC_TABLE_CLASS_H

#include "mutable_table_view.h"
#include "table_data.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/ref.hpp>

namespace morphy
{

class BasicTable : public MutableTableView
{
    GDCLASS(BasicTable, MutableTableView);

    TableData<godot::String, 4> data;

protected:
    static void _bind_methods();

public:
    BasicTable();
    virtual ~BasicTable();

    virtual int64_t num_columns() const override;

    virtual int64_t num_rows() const override;

    virtual const godot::Variant& get_cell(int64_t p_column, int64_t p_row) const override;

    virtual void set_cell(int64_t p_column, int64_t p_row, const godot::Variant& p_value) override;

    void add_columns(const godot::PackedStringArray& p_names);

    void add_rows(int64_t p_num);
};

} // namespace morphy

#endif // MORPHY_BASIC_TABLE_CLASS_H
