#ifndef MORPHY_VIEWS_BASIC_TABLE_HPP
#define MORPHY_VIEWS_BASIC_TABLE_HPP

#include <views/mutable_table_view.hpp>
#include <views/table_data.hpp>

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace morphy {

class BasicTable : public MutableTableView {
	GDCLASS(BasicTable, MutableTableView);

	TableData<godot::String> data;

protected:
	static void _bind_methods();

public:
	BasicTable();
	virtual ~BasicTable();

	virtual uint64_t num_columns() const override;

	virtual uint64_t num_rows() const override;

	virtual uint64_t
	get_column_index(const godot::String &p_column_name) const override;

	virtual const godot::Variant &get_cell(uint64_t p_column, uint64_t p_row) const override;

	virtual void set_cell(uint64_t p_column, uint64_t p_row, const godot::Variant &p_value) override;

	void add_columns(const godot::PackedStringArray &p_names);

	void add_rows(uint64_t p_num);
};

} // namespace morphy

#endif // MORPHY_VIEWS_BASIC_TABLE_HPP
