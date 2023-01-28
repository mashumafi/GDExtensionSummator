#include "register_types.h"
#include "table_view.h"
#include "expression_view.h"
#include "mutable_table_view.h"
#include "basic_table.h"
#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

void initialize_summator_types(godot::ModuleInitializationLevel p_level)
{
	if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	godot::ClassDB::register_class<morphy::TableView>();
	godot::ClassDB::register_class<morphy::MutableTableView>();

	godot::ClassDB::register_class<morphy::BasicTable>();

	godot::ClassDB::register_class<morphy::ExpressionColumn>();
	godot::ClassDB::register_class<morphy::ExpressionView>();
}

void uninitialize_summator_types(godot::ModuleInitializationLevel p_level) {
	if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C"
{

	// Initialization.

	GDExtensionBool GDE_EXPORT summator_library_init(
		const GDExtensionInterface *p_interface,
		const GDExtensionClassLibraryPtr p_library,
		GDExtensionInitialization *r_initialization)
	{
		godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

		init_obj.register_initializer(initialize_summator_types);
		init_obj.register_terminator(uninitialize_summator_types);
		init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}
