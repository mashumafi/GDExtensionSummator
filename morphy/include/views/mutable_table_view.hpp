#ifndef VIEWS_MUTABLE_TABLE_VIEW_HPP
#define VIEWS_MUTABLE_TABLE_VIEW_HPP

#include <views/table_view.hpp>

#include <godot_cpp/classes/ref.hpp>

namespace morphy {

class MutableTableView : public TableView {
	GDCLASS(MutableTableView, TableView);

protected:
	static void _bind_methods();

public:
	MutableTableView();
	virtual ~MutableTableView();

	virtual void set_cell(uint64_t p_column, uint64_t p_row, const godot::Variant &p_value);
};

} // namespace morphy

#endif // VIEWS_MUTABLE_TABLE_VIEW_HPP