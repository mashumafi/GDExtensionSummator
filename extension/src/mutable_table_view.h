#ifndef MORPHY_MUTABLE_TABLE_VIEW_CLASS_H
#define MORPHY_MUTABLE_TABLE_VIEW_CLASS_H

#include "table_view.h"

#include <godot_cpp/classes/ref.hpp>

namespace morphy
{

class MutableTableView : public TableView
{
    GDCLASS(MutableTableView, TableView);

protected:
    static void _bind_methods();

public:
    MutableTableView();
    virtual ~MutableTableView();

    virtual void set_cell(uint64_t p_column, uint64_t p_row, const godot::Variant& p_value);
};

} // namespace morphy

#endif // MORPHY_MUTABLE_TABLE_VIEW_CLASS_H