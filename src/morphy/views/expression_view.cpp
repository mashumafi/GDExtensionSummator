#include <util.hpp>
#include <views/expression_view.hpp>
#include <views/view_accessor.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace morphy {

namespace {

class DependencyTracker {
public:
	// Adds a dependency and returns true if it succeeds, otherwise there could be
	// a cycle
	bool add_dependency(uint64_t column, std::optional<uint64_t> row) {
		return depends_on.insert({ column, row }).second;
	}

private:
	GodotSet<ExpressionDependency> depends_on;
};

// Executes expressions recursively
// Tracks dependencies and detects cycles
struct RecursiveExpressionExecutor : public ITableView {
	virtual uint64_t num_columns() const {
		return data->num_columns();
	}

	virtual uint64_t num_rows() const {
		return data->num_rows();
	}

	virtual uint64_t get_column_index(const godot::String &p_column_name) const {
		// TODO: This logic is duplicated below in ExpressionView::get_column_index
		for (uint64_t column = 0; column < data->num_columns(); ++column) {
			if (data->get_header(column).name == p_column_name) {
				return column + (!view ? 0 : view->num_columns());
			}
		}

		ERR_FAIL_COND_V(!view, INVALID_INDEX);

		return view->get_column_index(p_column_name);
	}

	virtual const godot::Variant &get_cell(uint64_t column, uint64_t row) const {
		return data->get_cell(column, row);
	}

	ITableView *view = nullptr;
	ExpressionTable *data = nullptr;
	DependencyTracker dependency_tracker;
};

} //namespace

godot::Variant ExpressionHeader::execute(const godot::Array &p_inputs, godot::Object *p_base_instance) {
	godot::Variant result = expression->execute(p_inputs, p_base_instance);
	if (expression->has_execute_failed()) {
		godot::UtilityFunctions::push_error(expression->get_error_text());
	}

	return result;
}

godot::Error ExpressionColumn::set_name(const godot::String &p_name) {
	ERR_FAIL_COND_V_MSG(p_name.is_empty(), godot::FAILED, "Invalid column name");
	name = p_name;
	return godot::OK;
}

const godot::String &ExpressionColumn::get_name() const { return name; }

godot::Error ExpressionColumn::set_expression(const godot::String &p_expression) {
	godot::Ref<godot::Expression> expression;
	expression.instantiate();

	godot::PackedStringArray input_names;
	// TODO: Add custom inputs

	godot::Error error = expression->parse(p_expression, input_names);
	if (error != godot::OK) {
		godot::UtilityFunctions::push_error(expression->get_error_text());
		return error;
	}

	this->expression = expression;

	return godot::OK;
}

const godot::Ref<godot::Expression> &ExpressionColumn::get_expression() const {
	return expression;
}

void ExpressionColumn::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("set_name", "name"), &ExpressionColumn::set_name);
	godot::ClassDB::bind_method(godot::D_METHOD("set_expression", "expression"), &ExpressionColumn::set_expression);
}

ExpressionView::ExpressionView() {
}

ExpressionView::~ExpressionView() {
}

void ExpressionView::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("add_expressions", "expressions"), &ExpressionView::add_expressions);
	godot::ClassDB::bind_method(godot::D_METHOD("set_view", "view"), &ExpressionView::set_view);
}

uint64_t ExpressionView::num_columns() const {
	ERR_FAIL_COND_V(view.is_null(), data.num_columns());

	return view->num_columns() + data.num_columns();
}

uint64_t ExpressionView::num_rows() const {
	ERR_FAIL_COND_V(view.is_null(), 0);

	return view->num_rows();
}

uint64_t
ExpressionView::get_column_index(const godot::String &p_column_name) const {
	for (uint64_t column = 0; column < data.num_columns(); ++column) {
		if (data.get_header(column).name == p_column_name) {
			return column + (view.is_null() ? 0 : view->num_columns());
		}
	}

	ERR_FAIL_COND_V(view.is_null(), INVALID_INDEX);

	return view->get_column_index(p_column_name);
}

godot::Error ExpressionView::add_expressions(const godot::TypedArray<ExpressionColumn> &p_columns) {
	GodotVector<ExpressionHeader> headers;
	headers.reserve(p_columns.size());
	for (int i = 0; i < p_columns.size(); ++i) {
		const godot::Ref<ExpressionColumn> &expression_column = p_columns[i];
		ERR_FAIL_COND_V_MSG(expression_column->get_name().is_empty(), godot::FAILED, "Invalid column name");
		ERR_FAIL_COND_V_MSG(expression_column->get_expression().is_null(), godot::FAILED, "Invalid expression");

		headers.push_back({ expression_column->get_name(), expression_column->get_expression() });
	}

	data.add_columns(std::move(headers));

	if (view.is_null()) {
		return godot::OK;
	}

	data.add_rows(view->num_rows());

	auto base_instance = make_unique<ViewAccessor>();
	base_instance->set_view(this);
	for (uint64_t row = 0; row < data.num_rows(); ++row) {
		base_instance->set_current_row(row);

		for (uint64_t column = 0; column < data.num_columns(); ++column) {
			ExpressionHeader &header = data.get_header(column);
			godot::Array inputs;
			data.set_cell(column, row, header.execute(inputs, base_instance.get()));
		}
	}

	return godot::OK;
}

void ExpressionView::set_view(const godot::TypedArray<TableView> &p_view) {
	view = p_view[0];

	ERR_FAIL_COND_MSG(view.is_null(), "Invalid view");

	// TODO: Calculate expressions
}

const godot::Variant &ExpressionView::get_cell(uint64_t p_column,
		uint64_t p_row) const {
	static const godot::Variant nil;

	ERR_FAIL_COND_V_MSG(view.is_null(), nil, "No view");

	if (p_column < view->num_columns()) {
		return view->get_cell(p_column, p_row);
	} else {
		return data.get_cell(p_column - view->num_columns(), p_row);
	}
}

} //namespace morphy
