#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

attribute vec2 a_position;
attribute vec3 a_color;

varying vec3 v_color;

//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = vec4(a_position.xy, 1.0, 1.0);

    v_color = a_color;
}
//! [0]
