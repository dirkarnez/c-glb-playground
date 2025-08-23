#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Minimal .glb file for a cube (no textures, no animation, no materials, everything hardcoded).
// Cube vertices, indices, and GLB header/JSON/BIN chunk.

#pragma pack(push, 1)
typedef struct {
    uint32_t magic;      // 'glTF'
    uint32_t version;    // 2
    uint32_t length;     // file length
} GLBHeader;

typedef struct {
    uint32_t chunkLength;
    uint32_t chunkType;
} GLBChunkHeader;
#pragma pack(pop)

// Hardcoded cube geometry
float vertices[] = {
    // positions
    -1, -1, -1,  // 0
     1, -1, -1,  // 1
     1,  1, -1,  // 2
    -1,  1, -1,  // 3
    -1, -1,  1,  // 4
     1, -1,  1,  // 5
     1,  1,  1,  // 6
    -1,  1,  1   // 7
};

uint16_t indices[] = {
    // front
    0, 1, 2, 2, 3, 0,
    // back
    4, 5, 6, 6, 7, 4,
    // left
    0, 3, 7, 7, 4, 0,
    // right
    1, 5, 6, 6, 2, 1,
    // bottom
    0, 1, 5, 5, 4, 0,
    // top
    3, 2, 6, 6, 7, 3
};

// Hardcoded glTF JSON
const char *gltf_json =
"{\"asset\":{\"version\":\"2.0\"},"
"\"buffers\":[{\"byteLength\":272}],"
"\"bufferViews\":["
"{\"buffer\":0,\"byteOffset\":0,\"byteLength\":96},"
"{\"buffer\":0,\"byteOffset\":96,\"byteLength\":48}"
"],"
"\"accessors\":["
"{\"bufferView\":0,\"componentType\":5126,\"count\":8,\"type\":\"VEC3\"},"
"{\"bufferView\":1,\"componentType\":5123,\"count\":36,\"type\":\"SCALAR\"}"
"],"
"\"meshes\":[{\"primitives\":[{\"attributes\":{\"POSITION\":0},\"indices\":1}]}],"
"\"nodes\":[{\"mesh\":0}],"
"\"scenes\":[{\"nodes\":[0]}],"
"\"scene\":0"
"}";

int main() {
    FILE *f = fopen("cube.glb", "wb");
    if (!f) return 1;

    // Prepare binary chunk
    uint8_t bin[96 + 48] = {0};
    memcpy(bin, vertices, sizeof(vertices));
    memcpy(bin + 96, indices, sizeof(indices));

    // Align JSON chunk to 4 bytes
    size_t json_len = strlen(gltf_json);
    size_t json_pad = (4 - (json_len % 4)) % 4;
    size_t json_chunk_len = json_len + json_pad;

    // Align BIN chunk to 4 bytes
    size_t bin_len = sizeof(bin);
    size_t bin_pad = (4 - (bin_len % 4)) % 4;
    size_t bin_chunk_len = bin_len + bin_pad;

    // GLB header
    GLBHeader header;
    header.magic = 0x46546C67; // 'glTF'
    header.version = 2;
    header.length = sizeof(GLBHeader)
        + sizeof(GLBChunkHeader) + json_chunk_len
        + sizeof(GLBChunkHeader) + bin_chunk_len;

    // Write GLB header
    fwrite(&header, 1, sizeof(header), f);

    // Write JSON chunk header
    GLBChunkHeader jsonChunk;
    jsonChunk.chunkLength = (uint32_t)json_chunk_len;
    jsonChunk.chunkType = 0x4E4F534A; // 'JSON'
    fwrite(&jsonChunk, 1, sizeof(jsonChunk), f);

    // Write JSON chunk
    fwrite(gltf_json, 1, json_len, f);
    for (size_t i = 0; i < json_pad; ++i) fputc(' ', f);

    // Write BIN chunk header
    GLBChunkHeader binChunk;
    binChunk.chunkLength = (uint32_t)bin_chunk_len;
    binChunk.chunkType = 0x004E4942; // 'BIN'
    fwrite(&binChunk, 1, sizeof(binChunk), f);

    // Write BIN chunk
    fwrite(bin, 1, bin_len, f);
    for (size_t i = 0; i < bin_pad; ++i) fputc(0, f);

    fclose(f);
    printf("cube.glb written.\n");
    return 0;
}
