#ifndef MORPHY_VIEWS_SHADER_VIEW_HPP
#define MORPHY_VIEWS_SHADER_VIEW_HPP

#include <views/table_view.hpp>

namespace morphy {

class ShaderView : public TableView {
	GDCLASS(ShaderView, TableView);

public:
	ShaderView();
	virtual ~ShaderView();

	void add_column(const godot::String &name, const godot::String &compute_cell, const godot::TypedArray<godot::String> &depends);

protected:
	static void _bind_methods();

private:
};

} //namespace morphy

#endif