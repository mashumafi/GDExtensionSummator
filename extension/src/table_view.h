#ifndef MORPHY_TABLE_VIEW_CLASS_H
#define MORPHY_TABLE_VIEW_CLASS_H

#include <godot_cpp/classes/ref.hpp>

namespace morphy
{

class TableView : public godot::RefCounted
{
    GDCLASS(TableView, godot::RefCounted);

protected:
    static void _bind_methods();

public:
    TableView();
    virtual ~TableView();

    virtual uint64_t num_columns() const;

    virtual uint64_t num_rows() const;

    virtual uint64_t get_column_index(const godot::String& p_column_name) const;

    virtual const godot::Variant& get_cell(uint64_t column, uint64_t row) const;
};

} // namespace morphy

#endif // MORPHY_TABLE_VIEW_CLASS_H