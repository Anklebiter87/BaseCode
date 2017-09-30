#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>



int GetFileSize(FILE *handle)
{
    fseek(handle, 0L, SEEK_END);
    int filesize = ftell(handle);
    fseek(handle, 0L, SEEK_SET);
    return filesize;
}

int main(int argc, char** argv)
{
    FILE *filehandle;
    int filesize;
    char *cont;
    if(argc < 2)
    {
        printf("Supply a file\n");
        return 1;
    }



    filehandle = fopen(argv[1], "r");
    if(!filehandle)
    {   
        printf("Failed to open\n");
        return 1;
    }

    filesize = GetFileSize(filehandle);

    if(strcmp(argv[2],"calloc") == 0)
    {
        cont  = (char*)calloc(1, filesize+1);
        if(!cont)
        {
            printf("failed to map memory\n");
            return 1;
        }
        fread(cont, filesize, 1, filehandle);
        printf("file size: %d \n", filesize);
        free(cont);
        fclose(filehandle);

    }
    else if(strcmp(argv[2], "map") == 0)
    {
        int filenumber = fileno(filehandle);
        if(!filenumber)
        {
            printf("unable to get fd\n");
            return 1;
        }

        cont = (char*)mmap(0, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, filenumber, 0);
        if(cont == MAP_FAILED)
        {
            fclose(filehandle);
            printf("map failed\n");
            return 1;
        }
        printf("file size: %d \n", filesize);
       //printf("%s", cont);
        if(munmap(cont, filesize) == -1)
        {
            printf("failed to unmap\n");
        }
        fclose(filehandle);
    }
    return 0;
}

