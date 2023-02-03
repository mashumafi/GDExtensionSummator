#ifndef MORPHY_VIEWS_SHADER_VIEW_HPP
#define MORPHY_VIEWS_SHADER_VIEW_HPP

#include <views/table_view.hpp>

namespace morphy {

class ShaderView : public TableView {
	GDCLASS(ShaderView, TableView);

public:
	ShaderView();
	virtual ~ShaderView();

protected:
	static void _bind_methods();

private:
};

} //namespace morphy

#endif