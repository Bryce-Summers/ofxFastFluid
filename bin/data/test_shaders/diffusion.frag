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

    // Compute the neighbor locations.
    vec2 above = vec2(xy.x,     xy.y - 1);
    vec2 left  = vec2(xy.x - 1, xy.y);
    vec2 right = vec2(xy.x + 1, xy.y);
    vec2 below = vec2(xy.x, xy.y + 1);

    // Computer the neighbor texture values.
    vec3 c = texture(src_texture, xy).rgb;

    vec3 c1, c2, c3, c4;
    c1 = texture(src_texture, above).rgb;
    c2 = texture(src_texture, left ).rgb;
    c3 = texture(src_texture, right).rgb;
    c4 = texture(src_texture, below).rgb;

    // Modify values by the laplacian.
    float alpha = .0001;
    //vec3 color = c - .2*amount*(c*4 - c1 - c2 - c3 - c4);
    vec3 color = (c*alpha + c2 + c3 + c1 + c4)/(4 + alpha);

    output_color = vec4(color, 1);
}