#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char   *data;
    size_t length;
} buffer_t;

static size_t
callback_function(
        char *ptr,
        size_t chunk_size,
        size_t nmemb,
        void *user_data
)
{
    buffer_t *buffer = user_data;
    size_t total_size = chunk_size * nmemb;

    memcpy(buffer->data + buffer->length, ptr, total_size);
    buffer->length += total_size;
    return total_size;
}

int main(int argc, char *argv[]) {
    CURL *curl = curl_easy_init();
    if(curl) {
        CURLcode res;

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_function);

        buffer_t buffer;
        buffer.data = calloc(100*1024*1024, 1);
        buffer.length = 0;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
        res = curl_easy_perform(curl);

        char* title = strstr(buffer.data, "<title>");
        char* title_end = strstr(buffer.data, "</title>");
        *title_end = '\0';
        printf("%s\n", title + strlen("<title>"));

        free(buffer.data);
        curl_easy_cleanup(curl);
    }
}
