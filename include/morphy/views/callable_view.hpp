#ifndef MORPHY_VIEWS_CALLABLE_VIEW_HPP
#define MORPHY_VIEWS_CALLABLE_VIEW_HPP

#include <views/dependency_tracker.hpp>
#include <views/table_data.hpp>
#include <views/table_view.hpp>
#include <views/view_accessor.hpp>

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/callable.hpp>

namespace morphy {

struct CallableHeader {
	godot::String name;
	godot::Callable callable;

	godot::Variant execute(const godot::Array &arguments);
};

struct CallableMeta {
	// Keep a list so that if the formula is recomputed we can clear previous dependencies
	Dependencies dependencies;
};

class CallableColumn : public godot::RefCounted {
	GDCLASS(CallableColumn, godot::RefCounted);

public:
	godot::Error set_name(const godot::String &p_name);
	const godot::String &get_name() const;

	godot::Error set_callable(const godot::Callable &p_callable);
	const godot::Callable &get_callable() const;

protected:
	static void _bind_methods();

private:
	godot::String name;
	godot::Callable callable;
};

using CallableTable = TableData<CallableHeader, CallableMeta>;

class CallableView : public TableView {
	GDCLASS(CallableView, TableView);

public:
	CallableView();
	virtual ~CallableView();

	virtual uint64_t num_columns() const override;

	virtual uint64_t num_rows() const override;

	virtual uint64_t get_column_index(const godot::String &p_column_name) const override;

	virtual const godot::Variant &get_cell(uint64_t p_column, uint64_t p_row) const override;

	godot::Error add_callables(const godot::TypedArray<CallableColumn> &p_columns);

	void set_view(const godot::Ref<TableView> &p_view);

protected:
	static void _bind_methods();

private:
	godot::Ref<TableView> view;
	CallableTable data;
	// Helps figure out which Callables update when a dependency changes
	Requirements requirements;

	godot::Error compute_cell(ViewAccessor *base_instance, uint64_t column, uint64_t row);
};

} // namespace morphy

#endif // MORPHY_VIEWS_CALLABLE_VIEW_HPP