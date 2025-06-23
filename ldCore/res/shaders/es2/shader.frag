#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

varying lowp vec4 v_color;

//! [0]
void main()
{
    gl_FragColor = vec4(v_color.xyzw);

}
//! [0]

