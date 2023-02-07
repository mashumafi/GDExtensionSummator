#include <util.hpp>
#include <views/shader_view.hpp>

#include <godot_cpp/classes/rd_shader_file.hpp>
#include <godot_cpp/classes/rd_shader_source.hpp>
#include <godot_cpp/classes/rd_uniform.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace morphy {

static const char *shader_header = R"GLSL(
#version 450

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;

layout(set = 0, binding = 0, std430) restrict buffer table_t {
    float data[];
} table;
)GLSL";

static const char *row_begin = R"GLSL(
struct row_t {
)GLSL";

static const char *row_end = R"GLSL(
};
)GLSL";

static const char *shader_main_begin = R"GLSL(
void main() {
    row_t row;
)GLSL";

static const char *shader_main_end = R"GLSL(
    table.data[gl_GlobalInvocationID.x] = compute_cell(row);
}
)GLSL";

namespace {

float to_float(const godot::Variant &v) {
	switch (v.get_type()) {
		case godot::Variant::INT: {
			return (int)v;
		}
		default: {
			return v;
		}
	}
}

godot::Ref<godot::RDShaderSPIRV> create_shader(godot::RenderingDevice *rd, const godot::String &source) {
	godot::Ref<godot::RDShaderSource> shader_source;
	shader_source.instantiate();
	shader_source->set_language(godot::RenderingDevice::ShaderLanguage::SHADER_LANGUAGE_GLSL);
	shader_source->set_stage_source(godot::RenderingDevice::ShaderStage::SHADER_STAGE_COMPUTE, source);

	return rd->shader_compile_spirv_from_source(shader_source);
}

godot::RID create_shader(godot::RenderingDevice *rd, const godot::Ref<godot::RDShaderSPIRV> &shader_spirv) {
	godot::RID rid = rd->shader_create_from_spirv(shader_spirv);
	if (!rid.is_valid()) {
		godot::UtilityFunctions::push_error(shader_spirv->get_stage_compile_error(godot::RenderingDevice::ShaderStage::SHADER_STAGE_COMPUTE));
	}
	return rid;
}

godot::String create_source(const godot::String &compute_cell, const godot::TypedArray<godot::String> &depends) {
	godot::String source;
	source += shader_header;

	source += row_begin;
	for (int i = 0; i < depends.size(); ++i) {
		const godot::String &depend = depends[i];
		source += "float " + depend + ";";
	}
	source += row_end;

	source += compute_cell;

	source += shader_main_begin;
	for (int i = 0; i < depends.size(); ++i) {
		const godot::String &depend = depends[i];
		source += "row." + depend + " = table." + "data" + "[gl_GlobalInvocationID.x * " + godot::itos(depends.size()) + " + " + godot::itos(i) + "];";
	}
	source += shader_main_end;

	return source;
}

} //namespace

ShaderColumn::ShaderColumn() {
}

ShaderColumn::~ShaderColumn() {
}

void ShaderColumn::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("set_name", "name"), &ShaderColumn::set_name);
	godot::ClassDB::bind_method(godot::D_METHOD("set_shader", "compute_cell", "depends"), &ShaderColumn::set_shader);
}

void ShaderColumn::set_name(const godot::String p_name) {
	name = p_name;
}

void ShaderColumn::set_shader(const godot::String p_compute_cell, const godot::TypedArray<godot::String> p_depends) {
	auto RenderingServer = godot::RenderingServer::get_singleton();

	// Create a local RenderingDevice
	UniqueObject<godot::RenderingDevice> rd(RenderingServer->create_local_rendering_device());
	shader = create_shader(rd.get(), create_source(p_compute_cell, p_depends));
	ERR_FAIL_COND_MSG(shader.is_null(), "Could not create shader");
	depends = p_depends;
}

const godot::String &ShaderColumn::get_name() const {
	return name;
}

const godot::Ref<godot::RDShaderSPIRV> &ShaderColumn::get_shader() const {
	return shader;
}

const godot::PackedStringArray &ShaderColumn::get_depends() const {
	return depends;
}

ShaderView::ShaderView() {
}

ShaderView::~ShaderView() {
}

void ShaderView::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("add_columns", "columns"), &ShaderView::add_columns);
	godot::ClassDB::bind_method(godot::D_METHOD("set_view", "view"), &ShaderView::set_view);
}

void ShaderView::set_view(const godot::Ref<TableView> &p_view) {
	view = p_view;
}

const godot::Variant &ShaderView::get_cell(uint64_t p_column, uint64_t p_row) const {
	static const godot::Variant nil;

	ERR_FAIL_COND_V_MSG(view.is_null(), nil, "No view");

	if (p_column < view->num_columns()) {
		return view->get_cell(p_column, p_row);
	} else {
		return data.get_cell(p_column - view->num_columns(), p_row);
	}
}

void ShaderView::add_columns(const godot::TypedArray<ShaderColumn> &columns) {
	auto RenderingServer = godot::RenderingServer::get_singleton();

	// Create a local RenderingDevice
	for (int64_t i = 0; i < columns.size(); ++i) {
		const godot::Ref<ShaderColumn> &column = columns[i];
		const godot::PackedStringArray &depends = column->get_depends();
		UniqueObject<godot::RenderingDevice> rd(RenderingServer->create_local_rendering_device());

		godot::RID shader = create_shader(rd.get(), column->get_shader());

		godot::PackedInt64Array indices;
		indices.resize(depends.size());
		for (int64_t i = 0; i < depends.size(); ++i) {
			const godot::String &depend = depends[i];
			indices[i] = view->get_column_index(depend);
		}

		// Provide input data
		godot::PackedFloat32Array input;
		for (uint64_t row = 0; row < view->num_rows(); ++row) {
			for (uint64_t col = 0; col < indices.size(); ++col) {
				const godot::Variant var = view->get_cell(col, row);
				input.append(to_float(var));
			}
		}

		godot::PackedByteArray input_bytes = input.to_byte_array();

		godot::RID buffer = rd->storage_buffer_create(input_bytes.size(), input_bytes);

		godot::Ref<godot::RDUniform> uniform;
		uniform.instantiate();
		uniform->set_uniform_type(godot::RenderingDevice::UNIFORM_TYPE_STORAGE_BUFFER);
		uniform->set_binding(0);
		uniform->add_id(buffer);

		godot::Array uniforms;
		uniforms.push_back(uniform);

		godot::RID uniform_set = rd->uniform_set_create(uniforms, shader, 0);

		// Defining a compute pipeline
		godot::RID pipeline = rd->compute_pipeline_create(shader);
		int64_t compute_list = rd->compute_list_begin();
		rd->compute_list_bind_compute_pipeline(compute_list, pipeline);
		rd->compute_list_bind_uniform_set(compute_list, uniform_set, 0);
		rd->compute_list_dispatch(compute_list, (int)godot::Math::ceil(view->num_rows() / 32.0), 1, 1);
		rd->compute_list_end();

		// Execute a compute shader
		rd->submit();
		rd->sync();

		// Retrieving results
		godot::PackedByteArray output_bytes = rd->buffer_get_data(buffer);
		godot::PackedFloat32Array output = output_bytes.to_float32_array();

		// Cleanup
		rd->free_rid(pipeline);
		rd->free_rid(uniform_set);
		rd->free_rid(buffer);
		rd->free_rid(shader);

		// Store results
		ShaderHeader header;
		header.name = column->get_name();
		GodotVector<ShaderHeader> headers;
		headers.push_back(header);
		data.add_columns(headers);
		data.add_rows(view->num_rows());

		for (int64_t row = 0; row < data.num_rows(); ++row) {
			data.set_cell(0, row, output[row]);
		}
	}
}

} //namespace morphy