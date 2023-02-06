#include <util.hpp>
#include <views/shader_view.hpp>

#include <godot_cpp/classes/rd_shader_file.hpp>
#include <godot_cpp/classes/rd_shader_source.hpp>
#include <godot_cpp/classes/rd_shader_spirv.hpp>
#include <godot_cpp/classes/rd_uniform.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace morphy {

static const char *shader_header = R"GLSL(
#version 450

layout(local_size_x = 3, local_size_y = 1, local_size_z = 1) in;

layout(set = 0, binding = 0, std430) restrict buffer table_t {
    float data[];
} table;

struct row_t {
	float data;
};
)GLSL";

static const char *shader_main = R"GLSL(
void main() {
    row_t row;
	row.data = table.data[gl_GlobalInvocationID.x];
    table.data[gl_GlobalInvocationID.x] = compute_cell(row);
}
)GLSL";

namespace {

godot::RID create_shader(godot::RenderingDevice *rd, const godot::String &source) {
	godot::Ref<godot::RDShaderSource> shader_source;
	shader_source.instantiate();
	shader_source->set_language(godot::RenderingDevice::ShaderLanguage::SHADER_LANGUAGE_GLSL);
	shader_source->set_stage_source(godot::RenderingDevice::ShaderStage::SHADER_STAGE_COMPUTE, source);

	godot::Ref<godot::RDShaderSPIRV> shader_spirv = rd->shader_compile_spirv_from_source(shader_source);
	godot::RID rid = rd->shader_create_from_spirv(shader_spirv);
	if (!rid.is_valid()) {
		godot::UtilityFunctions::push_error(shader_spirv->get_stage_compile_error(godot::RenderingDevice::ShaderStage::SHADER_STAGE_COMPUTE));
	}
	return rid;
}

godot::String create_source(const godot::String &compute_cell, const godot::TypedArray<godot::String> &depends) {
	godot::String source;
	source += shader_header;
	source += compute_cell;
	source += shader_main;

	return source;
}

} //namespace

ShaderView::ShaderView() {
}

ShaderView::~ShaderView() {
}

void ShaderView::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("add_column", "name", "compute_cell", "depends"), &ShaderView::add_column);
}

void ShaderView::add_column(const godot::String &name, const godot::String &compute_cell, const godot::TypedArray<godot::String> &depends) {
	auto RenderingServer = godot::RenderingServer::get_singleton();

	// Create a local RenderingDevice
	UniqueObject<godot::RenderingDevice> rd(RenderingServer->create_local_rendering_device());
	//godot::RID shader = create_shader(rd.get());
	godot::RID shader = create_shader(rd.get(), create_source(compute_cell, depends));

	// Provide input data
	godot::PackedFloat32Array input;
	for (int i = 1; i < 16; ++i) {
		input.append(i);
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
	rd->compute_list_dispatch(compute_list, 5, 1, 1);
	rd->compute_list_end();

	// Execute a compute shader
	rd->submit();
	rd->sync();

	// Retrieving results
	godot::PackedByteArray output_bytes = rd->buffer_get_data(buffer);
	godot::PackedFloat32Array output = output_bytes.to_float32_array();
	godot::UtilityFunctions::print("Input:  ", input);
	godot::UtilityFunctions::print("Output: ", output);

	// Cleanup
	rd->free_rid(pipeline);
	rd->free_rid(uniform_set);
	rd->free_rid(buffer);
	rd->free_rid(shader);
}

} //namespace morphy