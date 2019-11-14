#ifndef WORDBOOOKLIB_H
#define WORDBOOOKLIB_H

#include <stdio.h>
#include "curlhelper.h"

#define API_BASE_URL "https://wordbook.cjpg.app"
#define API_PATH_DICTIONARIES "/dictionaries"
#define API_PATH_SUGGESTIONS "/suggestions"
#define API_PATH_DEFINITIONS "/definitions"

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
} *wordbook_dictionary_t;

/**
 * struct 'ARRAY' of the dictionaries!
 */
typedef struct wordbook_array_dictionary
{
    // array of dictionaries 
    wordbook_dictionary_t dicts;
    /**
     * current size of the '*dicts'
     * should be the same as count, but used for allocating memory
     * We cannot count on this being the actual count of wordbook_dictionary objects.
     */
    size_t size;
    // the current count of '*dicts', use this in loops! 
    size_t count;
} *wordbook_array_dictionary_t;

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
void wordbook_array_dictionary_free(wordbook_array_dictionary_t dict_array);

/** get all available dictionaries
 * 
 * NOTE* wordbook_array_dictionary countains pointers that needs
 *       to be freed before you exit, call 'wordbook_array_dictionary_free(...)'
 *
 * @returns a wordbook_array_dictionary
 * @see wordbook_array_dictionary
 */
wordbook_array_dictionary_t wordbook_get_dictionaries();

// initialize the array struct for 'wordbook_dictionary' eith a given size!
void initialize_array_wordbook_dictionary(wordbook_array_dictionary_t dict_array_struct_ptr, size_t initial_size);

// Insert a 'wordbook_dictionary' into an wordbook_array_dictionary
void insert_wordbook_dictionary(wordbook_array_dictionary_t dict_array_struct_ptr, struct wordbook_dictionary dict);

// [Internal]: free memory used by wordbook_dictionary props
void wordbook_dictionary_free_props(struct wordbook_dictionary dict);
// [Internal]: free memory used by wordbook_dictionary
void wordbook_dictionary_free(wordbook_dictionary_t dict_struct_ptr);

/** get suggestions from wordbook.cjpg.app, based on the query
 *
 * NOTE* that curl_download_result countains a pointer that needs
 *       to freed when you are done with it
 *
 * @returns a curl_download_result
 * @see curl_download_result
 */
curl_download_result wordbook_get_suggestions_json(const char *query);

/** get suggestions from wordbook.cjpg.app, based on the query and the selected dictionary
 *
 * NOTE* that curl_download_result countains a pointer that needs
 *       to freed when you are done with it
 *
 * @returns a curl_download_result
 * @see curl_download_result
 */
curl_download_result wordbook_get_suggestions_dict_json(const char *query, const char *dict_id);

#endif // !WORDBOOOKLIB_H