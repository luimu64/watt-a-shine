#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "curl.h"
#include <time.h>
#include <unistd.h>
#include <cjson/cJSON.h>

#define API_DOMAIN "https://api.spot-hinta.fi"
#define DEBUG_API_DOMAIN "http://localhost:3000"
#define POLLING_FREQUENCY 5

int main(int argc, char *argv[])
{
    struct MemoryStruct chunk;
    CURL *handle;
    CURLcode res;
    chunk.memory = malloc(1);
    chunk.size = 0;
    cJSON *todayData = NULL;
    cJSON *tomorrowData = NULL;

    time_t now;
    struct tm *lt;
    int last_hour = 23;

    handle = curl_easy_init();
    if (!handle)
        exit(1);

    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);

    while (1)
    {
        // sync time to system time
        time(&now);
        lt = localtime(&now);
        printf("%d:%d:%d %d\n", lt->tm_hour, lt->tm_min, lt->tm_sec, last_hour);
        // fetch next day data at 2pm
        if (lt->tm_hour == 14 && tomorrowData == NULL)
        {
            // Perform the API request
            curl_easy_setopt(handle, CURLOPT_URL, DEBUG_API_DOMAIN "/DayForward.json");
            res = curl_easy_perform(handle);

            if (res != CURLE_OK)
                fprintf(stderr, "error: %s\n", curl_easy_strerror(res));
            else
                tomorrowData = cJSON_Parse(chunk.memory);

            printf("Data fetched successfully.\n");
        }

        // check if hour has changed since last update
        if ((last_hour < lt->tm_hour || (last_hour == 23 && lt->tm_hour == 0)) && todayData != NULL)
        {
            last_hour = lt->tm_hour;
            cJSON *item = cJSON_GetArrayItem(todayData, lt->tm_hour);
            printf("Current price: %f\n", cJSON_GetNumberValue(cJSON_GetObjectItemCaseSensitive(item, "PriceWithTax")));
        }

        // day changes, set next day data to be current data
        if (lt->tm_hour == 0 && tomorrowData != NULL)
        {
            todayData = tomorrowData;
            tomorrowData = NULL;
            printf("Data switched successfully.\n");
        }

        sleep(POLLING_FREQUENCY);
        fflush(stdout);
    }

    curl_easy_cleanup(handle);
    cJSON_Delete(todayData);
    cJSON_Delete(tomorrowData);
    free(chunk.memory);
}