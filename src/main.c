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
        printf("usage: %s <e|r> <input> <output>\n", argv[0]);
        return 1;
    }

    // check both 0 and 1 to be safe
    char mode = (argv[1][0] == '-') ? argv[1][1] : argv[1][0];

    if(mode=='e'){
	printf("starting extraction: %s -> %s\n", argv[2], argv[3]);
	int result = extract(argv[2],argv[3]);
	if(result==0) printf("extraction successful\n");
	return result;
    }

    // 'r'epack || 'p'ack
    if(mode == 'r' || mode == 'p') {
	printf("starting repack: %s -> %s\n",argv[2],argv[3]);
	int result = repack(argv[2],argv[3]);
	if(result==0) printf("repack successful\n");
	return result;
    }

    printf("unknown mode: %s. Use 'e' for extract or 'r' for repack.\n",argv[1]);
    return 1;
}
