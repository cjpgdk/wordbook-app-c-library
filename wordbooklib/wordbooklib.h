#ifndef WORDBOOOKLIB_H
#define WORDBOOOKLIB_H

#include <stdio.h>

#define API_BASE_URL "https://wordbook.cjpg.app"
#define API_PATH_DICTIONARIES "/dictionaries"
#define API_PATH_SUGGESTIONS "/suggestions"
#define API_PATH_DEFINITIONS "/definitions"

#ifndef WORDBOOK_LIB_USER_AGENT
// User-Agent to use in requests
#define WORDBOOK_LIB_USER_AGENT "C-Wordbook-Library/1.0"
#endif // !WORDBOOK_LIB_USER_AGENT




/**
 * struct for downloaded data!
 */
typedef struct curl_download_result_s {
    /**
     * pointer to a \0 terminated string of the data in memory
     */
    char *ptr;
    /**
     * the length 
     */
    size_t len;
} curl_download_result;

/**
 * struct for downloaded dictionaries!
 */
typedef struct wordbook_dictionary_s{
    /**
     * Dictionary id eg. 7-11
     */
    char *id;
    /**
     * Short name of the ictionary eg. tur-deu
     */
    char *short_name;
    /**
     * Full name of the ictionary eg. Turkish-German
     */
    char *long_name;
    /**
     * Url to the alphabet in this dictionary
     * eg. https://wordbook.cjpg.app/definitions?word=00databasealphabet&src_language_id=7&dest_language_id=11
     */
    char *alphabet;
    /**
     * Url to some extended info of this dictionary
     * eg. https://wordbook.cjpg.app/definitions?word=00databaseinfo&src_language_id=7&dest_language_id=11
     */
    char *info;
    /**
     * Url to the url of the publisher of this dictionary
     * eg. https://wordbook.cjpg.app/definitions?word=00databaseurl&src_language_id=7&dest_language_id=11
     */
    char *url;
} wordbook_dictionary;

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

// 

/** download all available dictionaries from wordbook.cjpg.app. 
 *
 * NOTE* that curl_download_result countains a pointer that needs
 *       to freed when you are done with it
 * 
 * @returns a curl_download_result
 * @see curl_download_result
 */
curl_download_result wordbook_get_dictionaries_json();



#endif // !WORDBOOOKLIB_H