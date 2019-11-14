#ifndef CURLHELPER_H
#define CURLHELPER_H

#include <stdio.h>

#ifndef WORDBOOK_LIB_USER_AGENT
// User-Agent to use in requests
#define WORDBOOK_LIB_USER_AGENT "C-Wordbook-Library/1.0"
#endif // !WORDBOOK_LIB_USER_AGENT

/**
 * struct for downloaded data!
 */
typedef struct curl_download_result
{
    /**
     * pointer to a \0 terminated string of the data in memory
     */
    char *ptr;
    /**
     * the length
     */
    size_t len;
} curl_download_result;


/** perform a get request.
 *
 * NOTE* that curl_download_result countains a pointer that needs
 *       to freed when you are done with it
 *
 * @param url url to request
 * @returns a curl_download_result
 * @see curl_download_result
 */
curl_download_result wordbook_perform_http_get(const char *url);
// function used with: CURLOPT_WRITEFUNCTION
size_t wordbook_curl_write_function(void  *ptr, size_t size, size_t nmemb, void *download_result);
// function used to initialize the curl_download_result
void initialize_dl_result(void *download_result);

#endif // !CURLHELPER_H
