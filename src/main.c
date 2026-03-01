#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto.h"
#include "compress.h"

unsigned char* read_file(const char *filename, size_t *out_size) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    unsigned char *buf = malloc(size);
    if (buf) fread(buf, 1, size, f);
    fclose(f);
    *out_size = (size_t)size;
    return buf;
}

int extract(char *in, char *out) {
    size_t size;
    unsigned char *data = read_file(in, &size);
    if (!data) return 1;

    rc4_operation(data, (int)size);
    
    // Note: If Gzip starts at offset 12, use: data + 12, size - 12
    mem_result res = decompress_from_memory(data, size);
    free(data);

    if (res.error != 0) { printf("Decompression failed: %d\n", res.error); return 1; }

    FILE *f = fopen(out, "wb");
    fwrite(res.data, 1, res.size, f);
    fclose(f);
    free(res.data);
    return 0;
}

int repack(char *in, char *out) {
    size_t size;
    unsigned char *data = read_file(in, &size);
    if (!data) return 1;

    // compress the plaintext
    mem_result res = compress_from_memory(data, size);
    free(data);
    if (res.error != 0) return 1;

    // encrypt the compressed result
    rc4_operation(res.data, (int)res.size);

    FILE *f = fopen(out, "wb");
    fwrite(res.data, 1, res.size, f);
    fclose(f);
    free(res.data);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <e|r> <input> <output>\n", argv[0]);
        return 1;
    }
    if (argv[1][1] == 'e') return extract(argv[2], argv[3]);
    if (argv[1][1] == 'r') return repack(argv[2], argv[3]);
    return 1;
}
