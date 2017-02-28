/*
 * Divergence Program.
 * Written by Bryce Summers on 2 - 10 - 2017.
 */

#version 150

uniform float halfrdx;   // .5 / grid scale.
uniform sampler2DRect w; // Temporary pre-projected vector field.

out vec4 color; // output: Divergence.

void main()
{
    vec2 coords = gl_FragCoord.xy;

    vec4 left  = texture(w, coords - vec2(1, 0));
    vec4 right = texture(w, coords + vec2(1, 0));
    vec4 down  = texture(w, coords - vec2(0, 1));
    vec4 up    = texture(w, coords + vec2(0, 1));

    float div = halfrdx * ((right.x - left.x) + (up.y - down.y));

    color = vec4(div, div, div, 1);
}