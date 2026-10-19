#version 110

varying vec4 Color;
varying float Light;
uniform float shadowLift;

void main() {
    float light = Light + shadowLift * (1.0 - smoothstep(0.0, 0.5, Light)) * (1.0 - Light);
    gl_FragColor = vec4(Color.rgb * light, Color.a);
}
