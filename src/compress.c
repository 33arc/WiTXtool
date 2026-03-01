#include "compress.h"
#include <zlib.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK 16384

mem_result decompress_from_memory(const unsigned char *input, size_t input_size) {
    mem_result result = {NULL, 0, 0};
    z_stream strm = {0};
    strm.avail_in = (uInt)input_size;
    strm.next_in = (Bytef *)input;

    // 15 + 32 enables automatic header detection (Zlib/Gzip)
    int ret = inflateInit2(&strm, 15 + 32);
    if (ret != Z_OK) { result.error = ret; return result; }

    unsigned char *decompressed = NULL;
    size_t total_out = 0;

    do {
        unsigned char outbuf[CHUNK];
        strm.avail_out = CHUNK;
        strm.next_out = outbuf;

        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret < 0 && ret != Z_BUF_ERROR) { result.error = ret; goto cleanup; }

        size_t have = CHUNK - strm.avail_out;
        if (have > 0) {
            unsigned char *tmp = realloc(decompressed, total_out + have);
            if (!tmp) { result.error = -2; goto cleanup; }
            decompressed = tmp;
            memcpy(decompressed + total_out, outbuf, have);
            total_out += have;
        }
    } while (ret != Z_STREAM_END);

    result.data = decompressed;
    result.size = total_out;

cleanup:
    inflateEnd(&strm);
    if (result.error != 0) { free(decompressed); result.data = NULL; }
    return result;
}

mem_result compress_from_memory(const unsigned char *input, size_t input_size) {
    mem_result result = {NULL, 0, 0};
    z_stream strm = {0};
    strm.avail_in = (uInt)input_size;
    strm.next_in = (Bytef *)input;

    // 15 + 16 forces Gzip wrapper
    int ret = deflateInit2(&strm, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK) { result.error = ret; return result; }

    unsigned char *compressed = NULL;
    size_t total_out = 0;

    do {
        unsigned char outbuf[CHUNK];
        strm.avail_out = CHUNK;
        strm.next_out = outbuf;

        // Use Z_FINISH to tell Zlib this is the only buffer
        ret = deflate(&strm, Z_FINISH);

        size_t have = CHUNK - strm.avail_out;
        if (have > 0) {
            unsigned char *tmp = realloc(compressed, total_out + have);
            if (!tmp) { result.error = -2; goto cleanup; }
            compressed = tmp;
            memcpy(compressed + total_out, outbuf, have);
            total_out += have;
        }
    } while (ret != Z_STREAM_END);

    result.data = compressed;
    result.size = total_out;

cleanup:
    deflateEnd(&strm);
    if (result.error != 0) { free(compressed); result.data = NULL; }
    return result;
}


