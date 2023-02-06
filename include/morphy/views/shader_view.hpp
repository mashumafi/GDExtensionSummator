#ifndef MORPHY_VIEWS_SHADER_VIEW_HPP
#define MORPHY_VIEWS_SHADER_VIEW_HPP

#include <views/table_view.hpp>
#include <views/table_data.hpp>

#include <godot_cpp/classes/rd_shader_spirv.hpp>

namespace morphy {

struct ShaderHeader {
	godot::String name;
	godot::RDShaderSPIRV shader;
};

class ShaderView : public TableView {
	GDCLASS(ShaderView, TableView);

public:
	ShaderView();
	virtual ~ShaderView();

	void set_view(const godot::Ref<TableView> &p_view);
	virtual const godot::Variant &get_cell(uint64_t p_column, uint64_t p_row) const override;

	void add_column(const godot::String &name, const godot::String &compute_cell, const godot::TypedArray<godot::String> &depends);

protected:
	static void _bind_methods();

private:
	godot::Ref<TableView> view;
	TableData<ShaderHeader> data;
};

} //namespace morphy

#endif