#ifndef MORPHY_COLUMN_ACCESSOR_CLASS_H
#define MORPHY_COLUMN_ACCESSOR_CLASS_H

#include "table_view.h"

#include <godot_cpp/classes/object.hpp>

namespace morphy
{

class ColumnAccessor : public godot::Object
{
    GDCLASS(ColumnAccessor, godot::Object);

public:
    void set_view(TableView* p_view);
    void set_current_column(uint64_t p_current_column);
    const godot::Variant& cell(uint64_t cell) const;

protected:
    static void _bind_methods();

private:
    TableView* view;
    uint64_t current_column = 0;
};

} // namespace morphy

#endif // MORPHY_COLUMN_ACCESSOR_CLASS_H
