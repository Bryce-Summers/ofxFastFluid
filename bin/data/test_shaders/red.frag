#version 150

uniform float screen_width;
uniform float screen_height;

out vec4 outputColor;

void main()
{
    // gl_FragCoord contains the window relative coordinate for the fragment.
    // we use gl_FragCoord.x position to control the red color value.
    // we use gl_FragCoord.y position to control the green color value.
    // please note that all r, g, b, a values are between 0 and 1.
    
    vec2 xy     = gl_FragCoord.xy;
    vec2 center = vec2(screen_width/2, screen_height/2);

    // Black if not in circle.
    outputColor = vec4(1, 0, 0, 1);

}