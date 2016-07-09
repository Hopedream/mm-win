#pragma once

typedef unsigned int uint32;
typedef struct
{
    unsigned int total[2];
    uint32 state[4];
    unsigned char buffer[64];
}
md5_context;

void md5_starts( md5_context *ctx );
void md5_update( md5_context *ctx, unsigned char*input, uint32 length );
void md5_finish( md5_context *ctx, unsigned char digest[16] );
char *md5_string(const unsigned char *in, char *out, uint32 in_len);
char *md5_convert(char* in, char out[16]);
