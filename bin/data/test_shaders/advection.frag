#version 150

uniform float timestep;  // Timestep.
uniform float rdx; // 1 / grid scale.
uniform sampler2DRect v; // Velocities that we will advect by.
uniform sampler2DRect x; // Quantity to advect.

out vec4 xNew;

void main()
{
    vec2 coords = gl_FragCoord.xy;
    vec2 pos    = coords - timestep * rdx * (texture(v, coords).xy - vec2(.5, .5));

    xNew = texture(x, pos);
}