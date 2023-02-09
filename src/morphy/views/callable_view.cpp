#include <util.hpp>
#include <views/callable_view.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <utility>

namespace morphy {

godot::Variant CallableHeader::execute(const godot::Array &arguments) {
	return callable.callv(arguments);
}

godot::Error CallableColumn::set_name(const godot::String &p_name) {
	ERR_FAIL_COND_V_MSG(p_name.is_empty(), godot::FAILED, "Invalid column name");
	name = p_name;
	return godot::OK;
}

const godot::String &CallableColumn::get_name() const {
	return name;
}

godot::Error CallableColumn::set_callable(const godot::Callable &p_callable) {
	callable = p_callable;

	return godot::OK;
}

const godot::Callable &CallableColumn::get_callable() const {
	return callable;
}

void CallableColumn::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("set_name", "name"), &CallableColumn::set_name);
	godot::ClassDB::bind_method(godot::D_METHOD("set_callable", "callable"), &CallableColumn::set_callable);
}

CallableView::CallableView() {
}

CallableView::~CallableView() {
}

void CallableView::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("add_callables", "callables"), &CallableView::add_callables);
	godot::ClassDB::bind_method(godot::D_METHOD("set_view", "view"), &CallableView::set_view);
}

uint64_t CallableView::num_columns() const {
	ERR_FAIL_COND_V(view.is_null(), data.num_columns());

	return view->num_columns() + data.num_columns();
}

uint64_t CallableView::num_rows() const {
	ERR_FAIL_COND_V(view.is_null(), 0);

	return view->num_rows();
}

uint64_t CallableView::get_column_index(const godot::String &p_column_name) const {
	for (uint64_t column = 0; column < data.num_columns(); ++column) {
		if (data.get_header(column).name == p_column_name) {
			return column + (view.is_null() ? 0 : view->num_columns());
		}
	}

	ERR_FAIL_COND_V(view.is_null(), INVALID_INDEX);

	return view->get_column_index(p_column_name);
}

godot::Error CallableView::compute_cell(ViewAccessor *base_instance, uint64_t column, uint64_t row) {
	base_instance->set_current_row(row);

	CallableMeta *meta = data.get_cell_meta(column, row);
	ERR_FAIL_COND_V_MSG(!meta, godot::FAILED, "No meta data found");

	// Clear previous dependencies
	for (const Dependency &dependency : meta->dependencies) {
		auto requirement = requirements.find(dependency);
		if (requirement != requirements.end()) {
			requirement->second.erase({ column, row });
			if (requirement->second.empty()) {
				requirements.erase(requirement);
			}
		}
	}

	// Track new dependencies
	DependencyTracker dependency_tracker;
	base_instance->set_dependency_tracker(&dependency_tracker);

	// Execute callable
	CallableHeader &header = data.get_header(column);
	godot::Array arguments;
	arguments.push_back(base_instance);
	data.set_cell(column, row, header.execute(arguments));

	// Store dependencies
	meta->dependencies = std::move(dependency_tracker.get_dependencies());
	for (const Dependency &dependency : meta->dependencies) {
		// Inserts into an existing requirement or creates an empty one
		requirements[dependency].insert({ column, row });
	}

	auto updateDependencies = [&](Requirements::iterator dependers) {
		if (dependers != requirements.end()) {
			// Snatching dependencies before they get destroyed
			DependencyOwners owners;
			for (const DependencyOwner &owner : std::exchange(dependers->second, owners)) {
				compute_cell(base_instance, owner.column, owner.row);
			}
		}
	};

	auto dependers = requirements.find({ column + view->num_columns(), row });
	updateDependencies(dependers);

	auto column_dependers = requirements.find({ column + view->num_columns(), std::nullopt });
	updateDependencies(column_dependers);

	return godot::OK;
}

godot::Error CallableView::add_callables(const godot::TypedArray<CallableColumn> &p_columns) {
	GodotVector<CallableHeader> headers;
	headers.reserve(p_columns.size());
	for (int i = 0; i < p_columns.size(); ++i) {
		const godot::Ref<CallableColumn> &callable_column = p_columns[i];
		ERR_FAIL_COND_V_MSG(callable_column->get_name().is_empty(), godot::FAILED, "Invalid column name");
		ERR_FAIL_COND_V_MSG(callable_column->get_callable().is_null(), godot::FAILED, "Invalid callable");

		headers.push_back({ callable_column->get_name(), callable_column->get_callable() });
	}

	data.add_columns(std::move(headers));

	if (view.is_null()) {
		return godot::OK;
	}

	data.add_rows(view->num_rows());

	auto base_instance = make_unique<ViewAccessor>();
	base_instance->set_view(this);

	for (uint64_t row = 0; row < data.num_rows(); ++row) {
		for (uint64_t column = 0; column < data.num_columns(); ++column) {
			compute_cell(base_instance.get(), column, row);
		}
	}

	return godot::OK;
}

void CallableView::set_view(const godot::Ref<TableView> &p_view) {
	view = p_view;

	// TODO: Calculate callables
}

const godot::Variant &CallableView::get_cell(uint64_t p_column, uint64_t p_row) const {
	static const godot::Variant nil;

	ERR_FAIL_COND_V_MSG(view.is_null(), nil, "No view");

	if (p_column < view->num_columns()) {
		return view->get_cell(p_column, p_row);
	} else {
		return data.get_cell(p_column - view->num_columns(), p_row);
	}
}

} //namespace morphy
