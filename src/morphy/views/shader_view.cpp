#include <util.hpp>
#include <views/shader_view.hpp>

#include <godot_cpp/classes/rd_shader_file.hpp>
#include <godot_cpp/classes/rd_shader_spirv.hpp>
#include <godot_cpp/classes/rd_uniform.hpp>
#include <godot_cpp/classes/rendering_device.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/rd_shader_source.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace morphy {

static const char* shader_source = R"GLSL(
#version 450

// Invocations in the (x, y, z) dimension
layout(local_size_x = 3, local_size_y = 1, local_size_z = 1) in;

// A binding to the buffer we create in our script
layout(set = 0, binding = 0, std430) restrict buffer MyDataBuffer {
    float data[];
}
my_data_buffer;

// The code we want to execute in each invocation
void main() {
    // gl_GlobalInvocationID.x uniquely identifies this invocation across all work groups
    my_data_buffer.data[gl_GlobalInvocationID.x] *= 2.0;
}
)GLSL";

godot::RID create_shader(godot::RenderingDevice* rd, const char* source) {
	godot::Ref<godot::RDShaderSource> shader_source;
	shader_source.instantiate();
	shader_source->set_language(godot::RenderingDevice::ShaderLanguage::SHADER_LANGUAGE_GLSL);
	shader_source->set_stage_source(godot::RenderingDevice::ShaderStage::SHADER_STAGE_COMPUTE, source);

	godot::Ref<godot::RDShaderSPIRV> shader_spirv = rd->shader_compile_spirv_from_source(shader_source);
	if (shader_spirv.is_null()) {
		godot::UtilityFunctions::print(shader_spirv->get_stage_compile_error(godot::RenderingDevice::ShaderStage::SHADER_STAGE_COMPUTE));
		return godot::RID();
	}
	return rd->shader_create_from_spirv(shader_spirv);
}

godot::RID create_shader(godot::RenderingDevice* rd) {
    auto ResourceLoader = godot::ResourceLoader::get_singleton();
	godot::Ref<godot::RDShaderFile> shader_file = ResourceLoader->load("res://cell.glsl");
	if (shader_file.is_null()) {
		godot::UtilityFunctions::print("Could not convert resource");
		return godot::RID();
	}
	godot::Ref<godot::RDShaderSPIRV> shader_spirv = shader_file->get_spirv();
	if (shader_spirv.is_null()) {
		godot::UtilityFunctions::print(shader_file->get_base_error());
		return godot::RID();
	}
	return rd->shader_create_from_spirv(shader_spirv);
}

ShaderView::ShaderView() {
	auto RenderingServer = godot::RenderingServer::get_singleton();

	// Create a local RenderingDevice
	UniqueObject<godot::RenderingDevice> rd(RenderingServer->create_local_rendering_device());
	//godot::RID shader = create_shader(rd.get());
	godot::RID shader = create_shader(rd.get(), shader_source);

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

ShaderView::~ShaderView() {
}

void ShaderView::_bind_methods() {
}

} //namespace morphy