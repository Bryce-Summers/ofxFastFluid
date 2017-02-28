#version 150

uniform float alpha;
uniform float rBeta; // Reciprocal beta.

uniform sampler2DRect x; // x vector (Ax = b)
uniform sampler2DRect b; // b vector (Ax = b)

out vec4 xNew;

void main()
{
    // gl_FragCoord contains the window relative coordinate for the fragment.
    // we use gl_FragCoord.x position to control the red color value.
    // we use gl_FragCoord.y position to control the green color value.
    // please note that all r, g, b, a values are between 0 and 1.
    
    vec2 xy    = gl_FragCoord.xy;

    vec4 xLeft   = texture(x, xy - vec2(1, 0));
    vec4 xRight  = texture(x, xy + vec2(1, 0));
    vec4 xBottom = texture(x, xy - vec2(0, 1));
    vec4 xTop    = texture(x, xy + vec2(0, 1));

    vec4 bCenter = texture(b, xy);

    xNew = (xLeft + xRight + xBottom + xTop + alpha*bCenter) * rBeta;
    xNew = vec4(xNew.rgb, 1);
}