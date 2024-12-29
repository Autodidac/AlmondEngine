#version 330 core

out vec4 FragColor;    // Final color output

in vec2 TexCoords;     // Texture coordinates from the vertex shader

uniform sampler2D textureSampler;  // The texture sampler (e.g., an atlas)
uniform vec2 texOffset;           // Texture offset within the atlas
uniform vec2 texSize;             // Size of the texture within the atlas

void main() {
    // Adjust the texture coordinates based on the atlas's offset and size
    vec2 adjustedTexCoords = TexCoords * texSize + texOffset;

    // Ensure the texture coordinates are within valid bounds (0.0 to 1.0)
    adjustedTexCoords = clamp(adjustedTexCoords, vec2(0.0), vec2(1.0));

    // Sample the texture at the adjusted coordinates
    FragColor = texture(textureSampler, adjustedTexCoords);
}
