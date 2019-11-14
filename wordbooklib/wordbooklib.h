#ifndef WORDBOOOKLIB_H
#define WORDBOOOKLIB_H

#include <stdio.h>
#include "curlhelper.h"

#define API_BASE_URL "https://wordbook.cjpg.app"
#define API_PATH_DICTIONARIES "/dictionaries"
#define API_PATH_SUGGESTIONS "/suggestions"
#define API_PATH_DEFINITIONS "/definitions"

/**
 * struct for downloaded suggestions!
 */
typedef struct wordbook_suggestion
{
    /**
     * Word id eg. 2139
     */
    int word_id;
    /**
     * The suggested word eg. 'hello'
     */
    char *word;
    /**
     * Language id eg. 1
     */
    int source_language_id;
    /**
     * Ful name of the soiurce language eg. English
     */
    char *source_language_name;
} *wordbook_suggestion_t;

/**
 * struct 'ARRAY' of the suggestions!
 */
typedef struct wordbook_array_suggestions
{
    // array of suggestions 
    wordbook_suggestion_t suggestions;
    /**
     * current size of the '*suggestions'
     * should be the same as count, but used for allocating memory
     * We cannot count on this being the actual count of wordbook_dictionary objects.
     */
    size_t size;
    // the current count of '*suggestions', use this in loops! 
    size_t count;
} *wordbook_array_suggestions_t;

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


/*****************************
 * Helpers!
 *****************************/

// get source language id from dict_id
int get_source_language_id_from_dict_id(const char *dict_id);
// get destination language id from dict_id
int get_destination_language_id_from_dict_id(const char *dict_id);

/*
 * dictionary methods!
 *
 * Methods marked as internal are only exposed becouse why not! But they are not ment to be used outside
 * the library.
 */


/** get all available dictionaries
 * 
 * NOTE* wordbook_array_dictionary countains pointers that needs
 *       to be freed before you exit, call 'wordbook_array_dictionary_free(...)'
 *
 * @returns a wordbook_array_dictionary
 * @see wordbook_array_dictionary
 */
wordbook_array_dictionary_t wordbook_get_dictionaries();
/** free memory used by the 'wordbook_array_dictionary'
 *
 * @param dict_array the wordbook_array_dictionary array struct to free from memory
 */
void wordbook_array_dictionary_free(wordbook_array_dictionary_t dict_array);
/** download all available dictionaries from wordbook.cjpg.app.
 *
 * NOTE* that curl_download_result countains a pointer that needs
 *       to freed when you are done with it
 *
 * @returns a curl_download_result with the raw json data.
 * @see curl_download_result
 */
curl_download_result wordbook_get_dictionaries_json();

// [Internal]: initialize the array struct for 'wordbook_dictionary' eith a given size!
void initialize_array_wordbook_dictionary(wordbook_array_dictionary_t dict_array_struct_ptr, size_t initial_size);
// [Internal]: Insert a 'wordbook_dictionary' into an wordbook_array_dictionary
void insert_wordbook_dictionary(wordbook_array_dictionary_t dict_array_struct_ptr, struct wordbook_dictionary dict);
// [Internal]: free memory used by the properties of wordbook_dictionary
void wordbook_dictionary_free_props(struct wordbook_dictionary dict);
// [Internal]: free memory used by wordbook_dictionary
void wordbook_dictionary_free(wordbook_dictionary_t dict_struct_ptr);


/*
 * suggestion methods!
 *
 * Methods marked as internal are only exposed becouse why not! But they are not ment to be used outside 
 * the library.
 */



/** get suggestions for a word or phrase.
 *
 * NOTE* wordbook_array_suggestions countains pointers that needs
 *       to be freed before you exit, call 'wordbook_array_suggestion_free(...)'
 *
 * @param query the word or phrase to lookup
 * @param dict_id the dictinoary id to use for the lookup, if NULL it's ignored
 * @returns a wordbook_array_suggestions
 * @see wordbook_array_suggestions
 */
wordbook_array_suggestions_t wordbook_get_suggestions(const char *query, const char *dict_id);
/** free memory used by the 'wordbook_array_suggestions'
 * 
 * @param suggest_array the wordbook_array_suggestions array struct to free from memory
 */
void wordbook_array_suggestion_free(wordbook_array_suggestions_t suggest_array);
/**  get suggestions from wordbook.cjpg.app, based on the query
 *
 * NOTE* that curl_download_result countains a pointer that needs
 *       to freed when you are done with it
 *
 * @param query the word or phrase to lookup
 * @returns a curl_download_result with the raw json data. Return is limited to 25 words!
 * @see curl_download_result
 */
curl_download_result wordbook_get_suggestions_json(const char *query);
/** get suggestions from wordbook.cjpg.app, based on the query and the selected dictionary
 *
 * NOTE* that curl_download_result countains a pointer that needs
 *       to freed when you are done with it
 *
 * @param query the word or phrase to lookup
 * @param dict_id the dictinoary id to use for the lookup, if NULL it's ignored
 * @returns a curl_download_result with the raw json data. Return is limited to 25 words!
 * @see curl_download_result 
 */
curl_download_result wordbook_get_dictionary_suggestions_json(const char *query, const char *dict_id);

// [Internal]: initialize the array struct for 'wordbook_suggestion' with a given size!
void initialize_array_wordbook_suggestions(wordbook_array_suggestions_t suggest_array_struct_ptr, size_t initial_size);
// [Internal]: free memory used by the properties in 'wordbook_suggestion'
void wordbook_suggestion_free_props(struct wordbook_suggestion suggest);
// [Internal]: Insert a 'wordbook_suggestion' into an wordbook_array_suggestions
void insert_wordbook_suggestion(wordbook_array_suggestions_t suggest_array_struct_ptr, struct wordbook_suggestion suggest);



/*
 * definition methods!
 *
 * Methods marked as internal are only exposed becouse why not! But they are not ment to be used outside
 * the library.
 */




// 
/** get definitions of a word from wordbook.cjpg.app
 *
 * If using the word to lookup the definition both source_language_id and destination_language_id must be present in the request!
 * 
 * NOTE* that curl_download_result countains a pointer that needs
 *       to freed when you are done with it
 *
 *
 * @param word_id The word id
 * @param word The word to lookup, if word_id is set it's ignored. Can be NULL
 * @param source_language_id The id of the source language
 * @param destination_language_id The id of the destination language
 * @returns a curl_download_result with the raw json data.
 * @see curl_download_result
 */
curl_download_result wordbook_get_dictionary_definitions_json(
    const int word_id, const char *word, const int source_language_id, const int destination_language_id);

#endif // !WORDBOOOKLIB_H