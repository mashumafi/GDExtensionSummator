#ifndef MORPHY_VIEWS_VIEW_ACCESSOR_HPP
#define MORPHY_VIEWS_VIEW_ACCESSOR_HPP

#include <util.hpp>
#include <views/column_accessor.hpp>
#include <views/table_view.hpp>

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/templates/hash_map.hpp>

namespace morphy {

class ViewAccessor : public godot::Object {
	GDCLASS(ViewAccessor, godot::Object);

public:
	ViewAccessor();

	void set_view(ITableView *p_view);
	void set_current_row(uint64_t p_current_row);

	uint64_t row() const;

protected:
	static void _bind_methods();

	ColumnAccessor *get_column(const godot::String &column_name);

private:
	ITableView *view;
	uint64_t current_row = 0;
	GodotUMap<GodotString, UniqueObject<ColumnAccessor>> column_cache;
};

} // namespace morphy

#endif // MORPHY_VIEWS_VIEW_ACCESSOR_HPP
