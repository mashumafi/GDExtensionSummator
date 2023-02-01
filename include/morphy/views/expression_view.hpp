#ifndef MORPHY_VIEWS_EXPRESSION_VIEW_HPP
#define MORPHY_VIEWS_EXPRESSION_VIEW_HPP

#include <util.hpp>
#include <views/dependency_tracker.hpp>
#include <views/table_data.hpp>
#include <views/table_view.hpp>
#include <views/view_accessor.hpp>

#include <godot_cpp/classes/expression.hpp>
#include <godot_cpp/classes/ref.hpp>

namespace morphy {

struct ExpressionHeader {
	godot::String name;
	godot::Ref<godot::Expression> expression;

	godot::Variant execute(const godot::Array &inputs = godot::Array(), godot::Object *base_instance = nullptr);
};

struct ExpressionMeta {
	// Keep a list so that if the formula is recomputed we can clear previous dependencies
	Dependencies dependencies;
};

class ExpressionColumn : public godot::RefCounted {
	GDCLASS(ExpressionColumn, godot::RefCounted);

public:
	godot::Error set_name(const godot::String &p_name);
	const godot::String &get_name() const;

	godot::Error set_expression(const godot::String &p_expression);
	const godot::Ref<godot::Expression> &get_expression() const;

protected:
	static void _bind_methods();

private:
	godot::String name;
	godot::Ref<godot::Expression> expression;
};

using ExpressionTable = TableData<ExpressionHeader, ExpressionMeta>;

class ExpressionView : public TableView {
	GDCLASS(ExpressionView, TableView);

public:
	ExpressionView();
	virtual ~ExpressionView();

	virtual uint64_t num_columns() const override;

	virtual uint64_t num_rows() const override;

	virtual uint64_t get_column_index(const godot::String &p_column_name) const override;

	virtual const godot::Variant &get_cell(uint64_t p_column, uint64_t p_row) const override;

	godot::Error add_expressions(const godot::TypedArray<ExpressionColumn> &p_columns);

	void set_view(const godot::Ref<TableView> &p_view);

protected:
	static void _bind_methods();

private:
	godot::Ref<TableView> view;
	ExpressionTable data;
	// Helps figure out which expressions update when a dependency changes
	Requirements requirements;

	godot::Error compute_cell(ViewAccessor *base_instance, uint64_t column, uint64_t row);
};

} // namespace morphy

#endif // MORPHY_VIEWS_EXPRESSION_VIEW_HPP