#include <stdio.h>
#include <string.h>
#include <zlib.h>

int main() {
    const char *text = "Hello zlib!";
    uLong src_len = strlen(text) + 1; // include null terminator
    uLong dest_len = compressBound(src_len);
    Bytef compressed[dest_len];

    int res = compress(compressed, &dest_len, (const Bytef *)text, src_len);
    if (res != Z_OK) {
        printf("Compression failed\n");
        return 1;
    }

    printf("Compression succeeded! Original: %lu, Compressed: %lu\n", src_len, dest_len);

    // Decompress
    Bytef decompressed[src_len];
    uLong decompressed_len = src_len;
    res = uncompress(decompressed, &decompressed_len, compressed, dest_len);
    if (res != Z_OK) {
        printf("Decompression failed\n");
        return 1;
    }

    printf("Decompression succeeded! Text: %s\n", decompressed);
    return 0;
}
