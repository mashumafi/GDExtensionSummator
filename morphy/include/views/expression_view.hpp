#ifndef VIEWS_EXPRESSION_VIEW_HPP
#define VIEWS_EXPRESSION_VIEW_HPP

#include <util.hpp>
#include <views/table_data.hpp>
#include <views/table_view.hpp>

#include <godot_cpp/classes/expression.hpp>
#include <godot_cpp/classes/ref.hpp>

#include <optional>

namespace morphy {

struct ExpressionDependency {
	uint64_t column;
	// Allow depending on an entire column
	std::optional<uint64_t> row;

	bool operator<(const ExpressionDependency &other) const {
		if (column < other.column) {
			return true;
		} else if (column > other.column) {
			return false;
		}

		if (!row && other.row) {
			return true;
		} else if (row && !other.row) {
			return false;
		}

		if (row.value() < other.row.value()) {
			return true;
		} else if (row.value() > other.row.value()) {
			return false;
		}

		return false; // They are equal
	}
};

struct ExpressionCell {
	uint64_t column;
	uint64_t row;

	bool operator<(const ExpressionCell &other) const {
		if (column < other.column) {
			return true;
		} else if (column > other.column) {
			return false;
		}

		if (row < other.row) {
			return true;
		} else if (row > other.row) {
			return false;
		}

		return false; // They are equal
	}
};

struct ExpressionHeader {
	godot::String name;
	godot::Ref<godot::Expression> expression;

	godot::Variant execute(const godot::Array &inputs = godot::Array(), godot::Object *base_instance = nullptr);
};

struct ExpressionMeta {
	bool computed = false;
	// Keep a list so that if the formula is recomputed we can clear previous
	// dependencies
	GodotSet<ExpressionDependency> depends_on;
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

	virtual uint64_t
	get_column_index(const godot::String &p_column_name) const override;

	virtual const godot::Variant &get_cell(uint64_t p_column, uint64_t p_row) const override;

	godot::Error
	add_expressions(const godot::TypedArray<ExpressionColumn> &p_columns);

	void set_view(const godot::TypedArray<TableView> &p_view);

protected:
	static void _bind_methods();

private:
	godot::Ref<TableView> view;
	ExpressionTable data;
	// Helps figure out which expressions update when a dependency changes
	GodotMap<ExpressionDependency, GodotSet<ExpressionCell>> dependencies;
};

} // namespace morphy

#endif // VIEWS_EXPRESSION_VIEW_HPP