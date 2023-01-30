#ifndef MORPHY_VIEW_ACCESSOR_CLASS_H
#define MORPHY_VIEW_ACCESSOR_CLASS_H

#include "table_view.h"
#include "column_accessor.h"
#include "util.h"

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include <string>
#include <unordered_map>

namespace morphy
{

class ViewAccessor : public godot::Object
{
    GDCLASS(ViewAccessor, godot::Object);

public:
    void set_view(ITableView* p_view);
    void set_current_row(uint64_t p_current_row);

    uint64_t row() const;

protected:
    static void _bind_methods();

    ColumnAccessor* get_column(const godot::String& column_name);

private:
    ITableView* view;
    uint64_t current_row = 0;
    std::unordered_map<std::u32string, UniqueObject<ColumnAccessor>> column_cache;
};

} // namespace morphy

#endif // MORPHY_VIEW_ACCESSOR_CLASS_H
