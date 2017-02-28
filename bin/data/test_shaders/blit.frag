#version 150

uniform float screen_width;
uniform float screen_height;
//uniform sampler2DRect src_texture;
uniform sampler2DRect src_texture;

out vec4 output_color;

void main()
{
    // gl_FragCoord contains the window relative coordinate for the fragment.
    // we use gl_FragCoord.x position to control the red color value.
    // we use gl_FragCoord.y position to control the green color value.
    // please note that all r, g, b, a values are between 0 and 1.
    vec2 xy    = gl_FragCoord.xy;// / screen_width;
    
    vec3 color = texture(src_texture, xy).rgb;

    output_color = vec4(color, 1);
}