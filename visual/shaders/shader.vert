#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform CameraBufferObject {
    mat4 view;
    mat4 proj;
} camera;

layout(set = 0, binding = 1) uniform ModelBufferObject {
    mat4 model;
    int texIndex;
} model;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) flat out int outTexIndex;
void main() {
//    gl_Position = vec4(inPosition, 3.0);
//    gl_Position = model.model * vec4(inPosition, 1.0);
    gl_Position = camera.proj * camera.view * model.model * vec4(inPosition, 1.0);
    outColor = inColor;
    outTexCoord = inTexCoord;
    outTexIndex = model.texIndex;
}