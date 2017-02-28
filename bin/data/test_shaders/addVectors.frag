#version 150

/*
 * Takes two textures that encode positive directions from [.5, 1.0]
 * and negative directions from [0, .5]
 * sums them together with this same representation.
 */

uniform sampler2DRect tex1;
uniform sampler2DRect tex2;

out vec4 output_sum;

void main()
{
    vec2 xy = gl_FragCoord.xy;

    vec3 v1 = texture(tex1, xy).xyz;
    vec3 v2 = texture(tex2, xy).xyz;

    vec3 translate = vec3(.5, .5, .5);

    v1 -= translate;
    v2 -= translate;

    vec3 sum = (v1 + v2);
    output_sum = vec4(sum + translate, 1);
}