#ifndef curl_h
#define curl_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MemoryStruct
{
    char *memory;
    size_t size;
};

size_t writeMemoryCallback(void *buffer, size_t size, size_t nmemb, void *userp);

#endif