#define CURL_STATICLIB
#include "wordbooklib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>


// function used with: CURLOPT_WRITEFUNCTION
size_t wordbook_curl_write_function(void *ptr, size_t size, size_t nmemb, curl_download_result *s)
{
    // calculate the length of the data
    size_t dlen = size * nmemb;
    size_t nlen = s->len + dlen;
    // call realloc to get the required amount of memory
    s->ptr = realloc(s->ptr, nlen + 1);
    // check if we failed
    if (s->ptr == NULL) {
        // exit if failed
        fprintf(stderr, "realloc() failed in curl_write_function\n");
        exit(EXIT_FAILURE);
    }
    // copy the source 'ptr' into our stuct ptr
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    // terminate the string
    s->ptr[nlen] = '\0';
    // set the new langth of the string
    s->len = nlen;
    return dlen;
}

// function used to initialize the struct curl_download_result
void initialize_dl_result(curl_download_result *s) {
    // set length to 0
    s->len = 0;
    // alocate a minimal chunk of memory
    s->ptr = malloc(s->len + 1);
    // check if we failed
    if (s->ptr == NULL) {
        fprintf(stderr, "malloc() failed in initialize_dl_result\n");
        exit(EXIT_FAILURE);
    }
    // terminate the string
    s->ptr[0] = '\0';
}

// download all available dictionaries from wordbook.cjpg.app. 
curl_download_result wordbook_get_dictionaries_json()
{
    return wordbook_perform_http_get(API_BASE_URL API_PATH_DICTIONARIES);
}

// perform an get request.
curl_download_result wordbook_perform_http_get(const char *url)
{
    CURL * curl;
    CURLcode res;
    curl_download_result s;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        initialize_dl_result(&s);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, WORDBOOK_LIB_USER_AGENT);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wordbook_curl_write_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }
    return s;
}