#version 450

layout(location=0) out vec4 frag_color;

in vec2 texture_coordinate;

layout(binding=0) uniform sampler2D render;

void main() {
    vec3 col = texture(render, texture_coordinate).rgb;
    frag_color = vec4(col, 1.0f);
}