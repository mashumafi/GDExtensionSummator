#ifndef MORPHY_TABLE_VIEW_CLASS_H
#define MORPHY_TABLE_VIEW_CLASS_H

#include <godot_cpp/classes/ref.hpp>

namespace morphy
{

struct ITableView
{
    virtual uint64_t num_columns() const = 0;

    virtual uint64_t num_rows() const = 0;

    virtual uint64_t get_column_index(const godot::String& p_column_name) const = 0;

    virtual const godot::Variant& get_cell(uint64_t column, uint64_t row) const = 0;
};

class TableView : public godot::RefCounted, public ITableView
{
    GDCLASS(TableView, godot::RefCounted);

protected:
    static void _bind_methods();

public:
    TableView();
    virtual ~TableView();

    virtual uint64_t num_columns() const override;

    virtual uint64_t num_rows() const override;

    virtual uint64_t get_column_index(const godot::String& p_column_name) const override;

    virtual const godot::Variant& get_cell(uint64_t column, uint64_t row) const override;
};

} // namespace morphy

#endif // MORPHY_TABLE_VIEW_CLASS_H