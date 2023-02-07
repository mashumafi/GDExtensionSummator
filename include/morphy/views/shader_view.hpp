#ifndef MORPHY_VIEWS_SHADER_VIEW_HPP
#define MORPHY_VIEWS_SHADER_VIEW_HPP

#include <views/table_data.hpp>
#include <views/table_view.hpp>

#include <godot_cpp/classes/rd_shader_spirv.hpp>

namespace morphy {

struct ShaderHeader {
	godot::String name;
	godot::Ref<godot::RDShaderSPIRV> shader;
};

class ShaderColumn : public godot::RefCounted {
	GDCLASS(ShaderColumn, RefCounted);

public:
	ShaderColumn();
	virtual ~ShaderColumn();

	void set_name(const godot::String p_name);
	void set_shader(const godot::String p_shader, const godot::TypedArray<godot::String> p_depends);

	const godot::String &get_name() const;
	const godot::Ref<godot::RDShaderSPIRV> &get_shader() const;
	const godot::PackedStringArray &get_depends() const;

protected:
	static void _bind_methods();

private:
	godot::String name;
	godot::Ref<godot::RDShaderSPIRV> shader;
	godot::PackedStringArray depends;
};

class ShaderView : public TableView {
	GDCLASS(ShaderView, TableView);

public:
	ShaderView();
	virtual ~ShaderView();

	void set_view(const godot::Ref<TableView> &p_view);
	virtual const godot::Variant &get_cell(uint64_t p_column, uint64_t p_row) const override;

	void add_columns(const godot::TypedArray<ShaderColumn> &columns);

protected:
	static void _bind_methods();

private:
	godot::Ref<TableView> view;
	TableData<ShaderHeader> data;
};

} //namespace morphy

#endif