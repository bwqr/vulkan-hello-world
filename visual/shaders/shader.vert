#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

layout(set = 0, binding = 1) uniform ModelBufferObject {
    mat4 model;
} model;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;
void main() {
    gl_Position = camera.proj * camera.view * model.model * vec4(inPosition, 1.0);
    outColor = inColor;
    outTexCoord = inTexCoord;
}