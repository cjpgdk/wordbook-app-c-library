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

/**
 * struct for downloaded dictionaries!
 */
typedef struct wordbook_dictionary
{
    /**
     * Dictionary id eg. 7-11
     */
    char *id;
    /**
     * Short name of the dictionary eg. tur-deu
     */
    char *short_name;
    /**
     * Full name of the dictionary eg. Turkish-German
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

/**
 * struct 'ARRAY' of the dictionaries!
 */
typedef struct wordbook_array_dictionary
{
    // array of dictionaries 
    wordbook_dictionary *dicts;
    /**
     * current size of the '*dicts'
     * should be the same as count, but used for allocating memory
     * We cannot count on this being the actual count of wordbook_dictionary objects.
     */
    size_t size;
    // the current count of '*dicts', use this in loops! 
    size_t count;
} wordbook_array_dictionary;

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

/** download all available dictionaries from wordbook.cjpg.app.
 *
 * NOTE* that curl_download_result countains a pointer that needs
 *       to freed when you are done with it
 *
 * @returns a curl_download_result
 * @see curl_download_result
 */
curl_download_result wordbook_get_dictionaries_json();

// free memory used by the 'wordbook_array_dictionary'
void wordbook_array_dictionary_free(wordbook_array_dictionary *dict_array);

/** get all available dictionaries
 * 
 * NOTE* wordbook_array_dictionary countains pointers that needs
 *       to be freed before you exit, call 'wordbook_array_dictionary_free(...)'
 *
 * @returns a wordbook_array_dictionary
 * @see wordbook_array_dictionary
 */
wordbook_array_dictionary *wordbook_get_dictionaries();

// initialize the array struct for 'wordbook_dictionary' eith a given size!
void initialize_array_wordbook_dictionary(wordbook_array_dictionary *dict_array, size_t initial_size);

// Insert a 'wordbook_dictionary' into an wordbook_array_dictionary
void insert_wordbook_dictionary(wordbook_array_dictionary *dict_array, wordbook_dictionary dict);

// [Internal]: free memory used by wordbook_dictionary props
void wordbook_dictionary_free_props(wordbook_dictionary dict);
// [Internal]: free memory used by wordbook_dictionary
void wordbook_dictionary_free(wordbook_dictionary *dict);

#endif // !WORDBOOOKLIB_H