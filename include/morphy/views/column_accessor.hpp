#ifndef MORPHY_VIEWS_COLUMN_ACCESSOR_HPP
#define MORPHY_VIEWS_COLUMN_ACCESSOR_HPP

#include <views/table_view.hpp>

#include <godot_cpp/classes/object.hpp>

namespace morphy {

class ColumnAccessor : public godot::Object {
	GDCLASS(ColumnAccessor, godot::Object);

public:
	void set_view(ITableView *p_view);
	void set_current_column(uint64_t p_current_column);
	const godot::Variant &cell(uint64_t cell) const;

protected:
	static void _bind_methods();

private:
	ITableView *view;
	uint64_t current_column = 0;
};

} // namespace morphy

#endif // MORPHY_VIEWS_COLUMN_ACCESSOR_HPP
