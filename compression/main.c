#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "zlib.h"


#define CHUNK 16384

int GetFileSize(FILE *handle)
{
    fseek(handle, 0L, SEEK_END);
    int filesize = ftell(handle);
    fseek(handle, 0L, SEEK_SET);
    return filesize;
}

int Compress(FILE *handle, void *map)
{
    int level = Z_BEST_COMPRESSION;
    int ret;
    int flush;
    z_stream strm;
    unsigned have;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];
    map = malloc(1); 
    assert(map == NULL);

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    int copied = 0;
    if(ret != Z_OK)
        return ret;

    do{
        strm.avail_in = fread(in, 1, CHUNK, handle);
        if(ferror(handle)){
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(handle) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
        do {
	       strm.avail_out = CHUNK;
	       strm.next_out = out;
	       ret = deflate(&strm, flush);
	       assert(ret != Z_STREAM_ERROR);
	       have = CHUNK - strm.avail_out;
           map = realloc(map, strm.total_out );
           assert(map == NULL);
           map = memcpy(map, out, have);
           assert(map == NULL);
/*
           if(fwrite(out, 1, have, map) != have || ferror(map)){
               (void)deflateEnd(&strm);
               return Z_ERRNO;
           }*/
        } while(strm.avail_out == 0);
        assert(strm.avail_in == 0);

    } while(flush != Z_FINISH);
    assert(ret == Z_STREAM_END);

    (void)deflateEnd(&strm);
    return strm.total_out;
}

int main(int argc, char** argv)
{
    FILE *filehandle;
    int filesize;
    char *cont;
    FILE *output;
    if(argc < 1)
    {
        printf("Supply a file\n");
        return 1;
    }

    filehandle = fopen(argv[1], "r");
    output = fopen("output", "wb");
    if(!filehandle || !output)
    {   
        printf("Failed to open\n");
        return 1;
    }

    filesize = GetFileSize(filehandle);

    int status = Compress(filehandle, cont);

    printf("file size: %d \n", filesize);
    fwrite(cont, 1, status, output);
    fclose(output);
    return 0;
}

