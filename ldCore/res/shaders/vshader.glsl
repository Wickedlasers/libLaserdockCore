#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

attribute vec4 a_position;
attribute vec4 a_color;

varying vec4 v_color;

//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = a_position;

    v_color = a_color;
}
//! [0]
