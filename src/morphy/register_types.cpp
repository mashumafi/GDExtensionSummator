#include <register_types.hpp>
#include <views/basic_table.hpp>
#include <views/column_accessor.hpp>
#include <views/expression_view.hpp>
#include <views/mutable_table_view.hpp>
#include <views/table_view.hpp>
#include <views/view_accessor.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include <gdextension_interface.h>

void initialize_morphy_types(godot::ModuleInitializationLevel p_level) {
	if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	godot::ClassDB::register_class<morphy::TableView>();
	godot::ClassDB::register_abstract_class<morphy::ViewAccessor>();
	godot::ClassDB::register_abstract_class<morphy::ColumnAccessor>();
	godot::ClassDB::register_class<morphy::MutableTableView>();

	godot::ClassDB::register_class<morphy::BasicTable>();

	godot::ClassDB::register_class<morphy::ExpressionColumn>();
	godot::ClassDB::register_class<morphy::ExpressionView>();
}

void uninitialize_morphy_types(godot::ModuleInitializationLevel p_level) {
	if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {

// Initialization.

GDExtensionBool GDE_EXPORT morphy_library_init(const GDExtensionInterface *p_interface, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

	init_obj.register_initializer(initialize_morphy_types);
	init_obj.register_terminator(uninitialize_morphy_types);
	init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
