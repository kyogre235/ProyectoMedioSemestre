#version 410 core
out vec4 FragColor;

in float vWorldY;
uniform float uMinY;
uniform float uMaxY;

void main()
{
    float denom = max(1e-6, uMaxY - uMinY);
    float t = clamp((vWorldY - uMinY) / denom, 0.0, 1.0);

    vec3 blue = vec3(0.0, 0.0, 1.0);
    vec3 yellow = vec3(1.0, 1.0, 0.0);
    vec3 red = vec3(1.0, 0.0, 0.0);

    vec3 color;
    if (t < 0.5) {
        float k = smoothstep(0.0, 0.5, t);
        color = mix(blue, yellow, k);
    } else {
        float k = smoothstep(0.5, 1.0, t);
        color = mix(yellow, red, k);
    }

    FragColor = vec4(color, 1.0);
}