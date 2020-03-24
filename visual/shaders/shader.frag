#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) flat in int inTexIndex;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 2) uniform sampler2D texSamplers[];

void main() {
    outColor = texture(texSamplers[nonuniformEXT(inTexIndex)], inTexCoord);
}