/*
 * Boundary Fragment shader.
 * Written by Bryce Summers on 2 - 10 - 2017.
 */

 #version 150

uniform sampler2DRect x; // The original velocity field.
uniform vec2 offset;
uniform float scale;


out vec4 color; // output.

void main()
{
    vec2 coords = gl_FragCoord.xy;

    vec2 val = texture(x, coords + offset).xy;

    vec2 translate = vec2(.5, .5);

    val -= translate;
    val *= scale;
    val += translate;

    color = vec4(val.x, val.y, 0, 1);
    
}