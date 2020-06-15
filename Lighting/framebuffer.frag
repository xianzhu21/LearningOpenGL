#version 420 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D screenTexture;
uniform float divider;

const float offset = 1.0 / 300.0;  

void main() {
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
    );

    // sharpen
    //float kernel[9] = float[](
    //    -1, -1, -1,
    //    -1,  9, -1,
    //    -1, -1, -1
    //);

    // blur
    //float kernel[9] = float[](
    //  1.0 / 16, 2.0 / 16, 1.0 / 16,
    //  2.0 / 16, 4.0 / 16, 2.0 / 16,
    //  1.0 / 16, 2.0 / 16, 1.0 / 16  
    //);

    // edge detection
    float kernel[9] = float[](
      1.0,  1.0,  1.0,
      1.0, -8.0,  1.0,
      1.0,  1.0,  1.0
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    //FragColor = vec4(col, 1.0);
    FragColor = texture(screenTexture, TexCoord);

    //if (gl_FragCoord.x < divider) {
    //  float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    //  FragColor = vec4(average, average, average, 1.0);
    //}
}