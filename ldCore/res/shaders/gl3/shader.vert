#version 140

in vec3 a_position;
in vec4 a_color;

//out vec3 position;
out vec4 position;
out vec4 v_color;

//! [0]
void main()
{
    v_color = vec4(a_color.xyz, 1.0);
    position = vec4(a_position.xy, 1.0, 1.0);
}
//! [0]
