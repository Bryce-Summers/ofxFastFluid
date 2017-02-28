/*
 * Gradient Subtraction Program.
 * Written by Bryce Summers on 2 - 10 - 2017.
 */

#version 150

uniform float halfrdx;   // .5 / grid scale.
uniform sampler2DRect p; // Pressure.
uniform sampler2DRect w; // Temporary pre-projected vector field.

out vec4 uNew; // output.

void main()
{
    vec2 coords = gl_FragCoord.xy;
    
    float left  = texture(p, coords - vec2(1, 0)).x;
    float right = texture(p, coords + vec2(1, 0)).x;
    float down  = texture(p, coords - vec2(0, 1)).x;
    float up    = texture(p, coords + vec2(0, 1)).x;

    vec2 val = texture(w, coords).xy;
    
    val -= halfrdx * vec2(right - left, up - down);

    uNew = vec4(val.x, val.y, 0, 1);
}