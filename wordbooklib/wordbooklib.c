
// todo: check return of growable_string_*

#define _CRT_SECURE_NO_WARNINGS
#include "growablestring.h"
#include "wordbooklib.h"
#include "curlhelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>

#if defined(WIN32) || defined(WIN64)
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

/*****************************
 * Helpers!
 *****************************/

// get destination language id from dict_id
int get_destination_language_id_from_dict_id(const char *dict_id)
{
    size_t len = strlen(dict_id);
    growable_string_t gstr = growable_string_new(len);
    if (gstr == NULL)
    {
        return 0;
    }
    size_t start_append = 0;
    for (size_t i = 0; i < len; i++)
    {
        if (start_append == 1)
        {
            int check = growable_string_append_char(gstr, dict_id[i]);
            if (check == 0)
            {
                return 0;
            }
        }
        else if (start_append == 0 && dict_id[i] == '-')
        {
            start_append = 1;
        }
    }
    int result = atoi(gstr->s);
    growable_string_delete(gstr);
    return result;
}

// get source language id from dict_id
int get_source_language_id_from_dict_id(const char *dict_id)
{
    size_t len = strlen(dict_id);
    growable_string_t gstr = growable_string_new(len);
    if (gstr == NULL)
    {
        return 0;
    }
    for (size_t i = 0; i < len; i++)
    {
        if (dict_id[i] == '-')
        {
            break;
        }
        int check = growable_string_append_char(gstr, dict_id[i]);
        if (check == 0)
        {
            return 0;
        }
    }
    int result = atoi(gstr->s);
    growable_string_delete(gstr);
    return result;
}


/*****************************
 * definition API methods!
 *****************************/

wordbook_array_definition_t wordbook_get_definitions(
    const int word_id,  /* The word id */
    const char *word, /* The word to lookup, if word_id is set it's ignored. Can be NULL */
    const int source_language_id,  /* The id of the source language */
    const int destination_language_id  /* The id of the destination language */)
{
    curl_download_result json_result;
    json_result = wordbook_get_dictionary_definitions_json(word_id, word, source_language_id, destination_language_id);

    // declare the array to hold the definitions
    wordbook_array_definition_t final_definitions_array = (wordbook_array_definition_t)malloc(sizeof(struct wordbook_array_definition));

    if (json_result.ptr != NULL)
    {
        // parse the json string
        struct json_object* json_obj;
        json_obj = json_tokener_parse(json_result.ptr);

        if (json_object_is_type(json_obj, json_type_object))
        {
            // initialize the definitions array, we cannot get the size so we assume size of 1
            initialize_array_wordbook_definitions(final_definitions_array, 1);

            struct json_object_iterator dictionaries_it;
            struct json_object_iterator dictionaries_it_end;

            dictionaries_it = json_object_iter_begin(json_obj);
            dictionaries_it_end = json_object_iter_end(json_obj);

            // loop the properties in the object, 
            while (!json_object_iter_equal(&dictionaries_it, &dictionaries_it_end))
            {
                // check the data and only get that type!
                struct json_object* definitions_array = json_object_iter_peek_value(&dictionaries_it);
                // each name is the id of a dictionary!
                //const char* name = json_object_iter_peek_name(&dictionaries_it);
                if (json_object_is_type(definitions_array, json_type_array))
                {
                    // the actual definition array, for a word, in the dictionary
                    size_t definition_length = json_object_array_length(definitions_array);
                    for (size_t i = 0; i < definition_length; i++)
                    {
                        // get the definition at index 'i'
                        json_object* definition = json_object_array_get_idx(definitions_array, i);

                        struct wordbook_definition struct_definition = (struct wordbook_definition){NULL, NULL, 0, 0, 0};

                        // the properties in this definition!
                        struct json_object_iterator definition_it;
                        struct json_object_iterator definition_it_end;
                        definition_it = json_object_iter_begin(definition);
                        definition_it_end = json_object_iter_end(definition);
                        while (!json_object_iter_equal(&definition_it, &definition_it_end))
                        {
                            // check that it's a string or an int, we do not care about any other data!
                            struct json_object* definition_value = json_object_iter_peek_value(&definition_it);
                            if (!json_object_is_type(definition_value, json_type_string) && !json_object_is_type(definition_value, json_type_int))
                            {
                                continue;
                            }

                            const char *definition_name = json_object_iter_peek_name(&definition_it);

                            if (strcasecmp(definition_name, "definition") == 0)
                            {
                                const char *value = json_object_get_string(definition_value);
                                struct_definition.definition = malloc(strlen(value) + 1);
                                if (struct_definition.definition == NULL)
                                {
                                    continue;
                                }
                                strcpy(struct_definition.definition, value);
                            }
                            else if (strcasecmp(definition_name, "dictionary") == 0)
                            {
                                const char *value = json_object_get_string(definition_value);
                                struct_definition.dictionary = malloc(strlen(value) + 1);
                                if (struct_definition.dictionary == NULL)
                                {
                                    continue;
                                }
                                strcpy(struct_definition.dictionary, value);
                            }
                            else if (strcasecmp(definition_name, "word_id") == 0)
                            {
                                struct_definition.word_id = json_object_get_int(definition_value);
                            }
                            else if (strcasecmp(definition_name, "dest_language_id") == 0)
                            {
                                struct_definition.destination_language_id = json_object_get_int(definition_value);
                            }
                            else if (strcasecmp(definition_name, "src_language_id") == 0)
                            {
                                struct_definition.source_language_id = json_object_get_int(definition_value);
                            }

                            // move to next object
                            json_object_iter_next(&definition_it);
                        }
                        // append and free.
                        insert_wordbook_definition(final_definitions_array, struct_definition);
                        wordbook_definition_free_props(struct_definition);
                    }

                }
                // move to next object
                json_object_iter_next(&dictionaries_it);
            }
        }
        json_object_put(json_obj);
        free(json_result.ptr);
    }

    return final_definitions_array;
}


// get definitions of a word from wordbook.cjpg.app
curl_download_result wordbook_get_dictionary_definitions_json(
    const int word_id,  /* The word id */
    const char *word, /* The word to lookup, if word_id is set it's ignored. Can be NULL */
    const int source_language_id,  /* The id of the source language */
    const int destination_language_id  /* The id of the destination language */)
{
    size_t len = strlen(API_BASE_URL API_PATH_DEFINITIONS "?");
    growable_string_t gstr = growable_string_new(len);
    growable_string_append_cstr(gstr, API_BASE_URL);
    growable_string_append_cstr(gstr, API_PATH_DEFINITIONS);
    if (word_id > 0)
    {
        growable_string_append_cstr(gstr, "?id=");
        growable_string_append_int(gstr, word_id);
    }
    else if (word != NULL)
    {
        growable_string_append_cstr(gstr, "?word=");
        growable_string_append_cstr(gstr, word);
    }
    else
    {
        fprintf(stderr, "wordbook_get_dictionary_definitions_json() invalid call missing word_id or word!\n");
        exit(EXIT_FAILURE);
    }

    if (source_language_id > 0)
    {
        growable_string_append_cstr(gstr, "&src_language_id=");
        growable_string_append_int(gstr, source_language_id);
    }
    if (destination_language_id > 0)
    {
        growable_string_append_cstr(gstr, "&dest_language_id=");
        growable_string_append_int(gstr, destination_language_id);
    }

    curl_download_result result = wordbook_perform_http_get(gstr->s);
    growable_string_delete(gstr);

    return result;
}


/*****************************
 * definitions array methods!
 *****************************/

 // free memory used by props in 'wordbook_definition'
 // make sure all props are NULL or a valid pointer.
void wordbook_definition_free_props(struct wordbook_definition definition)
{
    if (definition.definition)
    {
        free(definition.definition);
    }
    if (definition.dictionary)
    {
        free(definition.dictionary);
    }
}

 // Insert a 'wordbook_definition' into an wordbook_array_definition
void insert_wordbook_definition(wordbook_array_definition_t definition_array_struct_ptr, struct wordbook_definition definition)
{
    // increment the size, if needed.
    if (definition_array_struct_ptr->count == definition_array_struct_ptr->size)
    {
        definition_array_struct_ptr->size *= 2;
        struct wordbook_definition *tmp = (struct wordbook_definition *)realloc(
            definition_array_struct_ptr->definitions,
            definition_array_struct_ptr->size * sizeof(struct wordbook_suggestion)
        );

        if (tmp == NULL)
        {
            fprintf(stderr, "realloc() failed in insert_wordbook_suggestion\n");
            exit(EXIT_FAILURE);
        }

        definition_array_struct_ptr->definitions = tmp;
        // Initialize the last/new elements of the reallocated array
        for (size_t i = definition_array_struct_ptr->count; i < definition_array_struct_ptr->size; i++)
        {
            memset(&definition_array_struct_ptr->definitions[i], 0, sizeof(struct wordbook_suggestion));
        }
    }

    // simplify the code, by collecting the index.
    size_t index = definition_array_struct_ptr->count;

    // Copy the definition.
    size_t len = strlen(definition.definition) + 1;
    definition_array_struct_ptr->definitions[index].definition = malloc(len);
    if (definition_array_struct_ptr->definitions[index].definition != NULL)
    {
        strcpy(definition_array_struct_ptr->definitions[index].definition, definition.definition);
    }

    // Copy the dictionary.
    len = strlen(definition.dictionary) + 1;
    definition_array_struct_ptr->definitions[index].dictionary = malloc(len);
    if (definition_array_struct_ptr->definitions[index].dictionary != NULL)
    {
        strcpy(definition_array_struct_ptr->definitions[index].dictionary, definition.dictionary);
    }

    // Copy the word and lanuage id.
    definition_array_struct_ptr->definitions[index].word_id = definition.word_id;
    definition_array_struct_ptr->definitions[index].source_language_id = definition.source_language_id;
    definition_array_struct_ptr->definitions[index].destination_language_id = definition.destination_language_id;

    definition_array_struct_ptr->count++;
}


 // initialize the array struct for 'wordbook_definition' with a given size!
void initialize_array_wordbook_definitions(wordbook_array_definition_t definition_array_struct_ptr, size_t initial_size)
{
    // Allocate initial space
    definition_array_struct_ptr->definitions = (struct wordbook_definition *)malloc(initial_size * sizeof(struct wordbook_definition));
    definition_array_struct_ptr->count = 0;
    definition_array_struct_ptr->size = initial_size;

    // Initialize all values of the array to 0
    for (size_t i = 0; i < initial_size; i++)
    {
        // C6011
        if (definition_array_struct_ptr->definitions)
        {
            memset(&definition_array_struct_ptr->definitions[i], 0, sizeof(struct wordbook_definition));
        }
    }
}

/*****************************
 * suggestion API methods!
 *****************************/

// get suggestions for a word or phrase.
wordbook_array_suggestions_t wordbook_get_suggestions(const char *query, const char *dict_id)
{
    curl_download_result json_result;
    if (dict_id == NULL)
    {
        json_result = wordbook_get_suggestions_json(query);
    }
    else
    {
        json_result = wordbook_get_dictionary_suggestions_json(query, dict_id);
    }
    // declare the array to hold the suggestions
    wordbook_array_suggestions_t suggest_array = (wordbook_array_suggestions_t)malloc(sizeof(struct wordbook_array_suggestions));

    // make sure we got a pointer
    if (json_result.ptr != NULL)
    {
        // parse the json string
        struct json_object *json_obj;
        json_obj = json_tokener_parse(json_result.ptr);
        // get the type of data, making sure it's an object
        if (json_object_is_type(json_obj, json_type_object))
        {
            // extract the 'suggestions' property of the json object.
            struct json_object *suggestions_obj;
            json_bool suggestions_located = json_object_object_get_ex(json_obj, "suggestions", &suggestions_obj);
            if (suggestions_located == 0)
            {
                // unable to locate suggestions array....
                json_object_put(json_obj);
                free(json_result.ptr);
                free(suggest_array);
                return NULL;
            }

            size_t suggestions_count = json_object_array_length(suggestions_obj);

            // initialize the suggestions array
            initialize_array_wordbook_suggestions(suggest_array, suggestions_count);

            for (size_t i = 0; i < suggestions_count; i++)
            {
                 // get the array item at index 'i'
                json_object *obj = json_object_array_get_idx(suggestions_obj, i);

                // create a temporary dict.
                struct wordbook_suggestion suggest = (struct wordbook_suggestion){ 0,  NULL, 0, NULL };

                // lets just confirm this is an object
                if (json_object_is_type(obj, json_type_object))
                {
                    // all data related to a word is located in property "data"
                    struct json_object *word_data;
                    json_bool word_data_located = json_object_object_get_ex(obj, "data", &word_data);
                    if (word_data_located == 0)
                    {
                        // unable to locate data....
                        continue;
                    }

                    struct json_object_iterator it;
                    struct json_object_iterator it_end;

                    it = json_object_iter_begin(word_data);
                    it_end = json_object_iter_end(word_data);

                    // loop the properties in the object
                    while (!json_object_iter_equal(&it, &it_end))
                    {
                        // check the data and only get that type!
                        struct json_object *obj_val = json_object_iter_peek_value(&it);
                        if (json_object_is_type(obj_val, json_type_string))
                        {
                            const char *name = json_object_iter_peek_name(&it);
                            const char *value = json_object_get_string(obj_val);

                            // Copy language name
                            if (strcasecmp(name, "language") == 0)
                            {
                                suggest.source_language_name = malloc(strlen(value) + 1);
                                if (suggest.source_language_name == NULL)
                                {
                                    continue;
                                }
                                strcpy(suggest.source_language_name, value);
                            }
                            // Copy the word
                            else if (strcasecmp(name, "word") == 0)
                            {
                                suggest.word = malloc(strlen(value) + 1);
                                if (suggest.word == NULL)
                                {
                                    continue;
                                }
                                strcpy(suggest.word, value);
                            }
                            /*
                             * These are int's but, we can't trust that I will keep it that way!
                             */
                            // Copy the language_id
                            else if (strcasecmp(name, "language_id") == 0)
                            {
                                suggest.source_language_id = atoi(value);
                            }
                            // Copy the word_id
                            else if (strcasecmp(name, "word_id") == 0)
                            {
                                suggest.word_id = atoi(value);
                            }
                        }
                        else if (json_object_is_type(obj_val, json_type_int))
                        {
                            const char *name = json_object_iter_peek_name(&it);
                            int32_t value = json_object_get_int(obj_val);

                            // Copy the language_id
                            if (strcasecmp(name, "language_id") == 0)
                            {
                                suggest.source_language_id = value;
                            }
                            // Copy the word_id
                            else if (strcasecmp(name, "word_id") == 0)
                            {
                                suggest.word_id = value;
                            }
                        }
                        // move to next object
                        json_object_iter_next(&it);
                    }
                }

                // insert the new dict into the array.
                insert_wordbook_suggestion(suggest_array, suggest);
                // free memory used by temp dict
                wordbook_suggestion_free_props(suggest);
            }
        }
        // free the json object
        json_object_put(json_obj);
        // free the downloaded json.
        free(json_result.ptr);
    }
    return suggest_array;
}

// get suggestions from wordbook.cjpg.app, based on the query and the selected dictionary
curl_download_result wordbook_get_dictionary_suggestions_json(const char *query, const char *dict_id)
{
    size_t len = strlen(API_BASE_URL API_PATH_SUGGESTIONS "?");
    len += strlen(query);
    growable_string_t gstr = growable_string_new(len);
    growable_string_append_cstr(gstr, API_BASE_URL);
    growable_string_append_cstr(gstr, API_PATH_SUGGESTIONS);
    growable_string_append_cstr(gstr, "?query=");
    growable_string_append_cstr(gstr, query);
    if (dict_id != NULL)
    {
        growable_string_append_cstr(gstr, "&language=");
        growable_string_append_cstr(gstr, dict_id);
    }
    curl_download_result result = wordbook_perform_http_get(gstr->s);
    growable_string_delete(gstr);
    return result;
}

// get suggestions from wordbook.cjpg.app, based on the query
curl_download_result wordbook_get_suggestions_json(const char *query)
{
    return wordbook_get_dictionary_suggestions_json(query, NULL);
}


/*****************************
 * suggestions array methods!
 *****************************/

 // Insert a 'wordbook_suggestion' into an wordbook_array_suggestions
void insert_wordbook_suggestion(wordbook_array_suggestions_t suggest_array_struct_ptr, struct wordbook_suggestion suggest)
{
    // increment the size, if needed.
    if (suggest_array_struct_ptr->count == suggest_array_struct_ptr->size)
    {

        suggest_array_struct_ptr->size *= 2;

        wordbook_suggestion_t tmp = (wordbook_suggestion_t)realloc(
            suggest_array_struct_ptr->suggestions,
            suggest_array_struct_ptr->size * sizeof(struct wordbook_suggestion)
        );

        if (tmp == NULL)
        {
            fprintf(stderr, "realloc() failed in insert_wordbook_suggestion\n");
            exit(EXIT_FAILURE);
        }

        suggest_array_struct_ptr->suggestions = tmp;
        // Initialize the last/new elements of the reallocated array
        for (size_t i = suggest_array_struct_ptr->count; i < suggest_array_struct_ptr->size; i++)
        {
            memset(&suggest_array_struct_ptr->suggestions[i], 0, sizeof(struct wordbook_suggestion));
        }
    }

    // simplify the code, by collecting the index.
    size_t index = suggest_array_struct_ptr->count;

    // Copy the source_language_name.
    size_t len = strlen(suggest.source_language_name) + 1;
    suggest_array_struct_ptr->suggestions[index].source_language_name = malloc(len);
    if (suggest_array_struct_ptr->suggestions[index].source_language_name != NULL)
    {
        strcpy(suggest_array_struct_ptr->suggestions[index].source_language_name, suggest.source_language_name);
    }

    // Copy the word.
    len = strlen(suggest.word) + 1;
    suggest_array_struct_ptr->suggestions[index].word = malloc(len);
    if (suggest_array_struct_ptr->suggestions[index].word != NULL)
    {
        strcpy(suggest_array_struct_ptr->suggestions[index].word, suggest.word);
    }

    // Copy the word and lanuage id.
    suggest_array_struct_ptr->suggestions[index].word_id = suggest.word_id;
    suggest_array_struct_ptr->suggestions[index].source_language_id = suggest.source_language_id;

    suggest_array_struct_ptr->count++;
}

 // initialize the array struct for 'wordbook_suggestion' with a given size!
void initialize_array_wordbook_suggestions(wordbook_array_suggestions_t suggest_array_struct_ptr, size_t initial_size)
{
    // Allocate initial space
    suggest_array_struct_ptr->suggestions = (wordbook_suggestion_t)malloc(initial_size * sizeof(struct wordbook_suggestion));
    suggest_array_struct_ptr->count = 0;
    suggest_array_struct_ptr->size = initial_size;

    // Initialize all values of the array to 0
    for (size_t i = 0; i < initial_size; i++)
    {
        // C6011
        if (suggest_array_struct_ptr->suggestions)
        {
            memset(&suggest_array_struct_ptr->suggestions[i], 0, sizeof(struct wordbook_suggestion));
        }
    }
}


// free memory used by props in 'wordbook_suggestion'
// make sure all props are NULL or a valid pointer.
void wordbook_suggestion_free_props(struct wordbook_suggestion suggest)
{
    if (suggest.source_language_name)
    {
        free(suggest.source_language_name);
    }
    if (suggest.word)
    {
        free(suggest.word);
    }
}

// free memory used by the 'wordbook_array_suggestions'
void wordbook_array_suggestion_free(wordbook_array_suggestions_t suggest_array)
{
    if (suggest_array != NULL)
    {
        if (suggest_array->suggestions != NULL)
        {
            for (size_t i = 0; i < suggest_array->count; i++)
            {
                wordbook_suggestion_free_props(suggest_array->suggestions[i]);
            }
            free(suggest_array->suggestions);
        }
        free(suggest_array);
    }
}

/*****************************
 * dictionary API methods!
 *****************************/

 // get all available dictionaries
wordbook_array_dictionary_t wordbook_get_dictionaries()
{
    // get the raw json data
    curl_download_result json_result;
    json_result = wordbook_get_dictionaries_json();

    // declare the array to hold the dictionaries
    wordbook_array_dictionary_t dict_array = (wordbook_array_dictionary_t)malloc(sizeof(struct wordbook_array_dictionary));

    size_t dictionaries_count = 0;
    // make sure we got a pointer
    if (json_result.ptr != NULL)
    {
        // parse the json string
        struct json_object *json_obj;
        json_obj = json_tokener_parse(json_result.ptr);
        // get the type of data, making sure it's an array
        if (json_object_is_type(json_obj, json_type_array))
        {
            // get the dictionary counter.
            dictionaries_count = json_object_array_length(json_obj);

            // initialize the dictionary array
            initialize_array_wordbook_dictionary(dict_array, dictionaries_count);

            // loop the items in the array
            for (size_t i = 0; i < dictionaries_count; i++)
            {
                /*
                 * Finaly we got the actual dictionary object!
                 */
                 // get the array item at index 'i'
                json_object *obj = json_object_array_get_idx(json_obj, i);

                // create a temporary dict.
                struct wordbook_dictionary dict = (struct wordbook_dictionary){ NULL,  NULL, NULL, NULL, NULL, NULL };

                // lets just confirm this is an object
                if (json_object_is_type(obj, json_type_object))
                {
                    struct json_object_iterator it;
                    struct json_object_iterator it_end;

                    it = json_object_iter_begin(obj);
                    it_end = json_object_iter_end(obj);

                    // loop the properties in the object
                    while (!json_object_iter_equal(&it, &it_end))
                    {
                        // this must be strings so we check to ignore any other data!
                        struct json_object *obj_val = json_object_iter_peek_value(&it);
                        if (json_object_is_type(obj_val, json_type_string))
                        {
                            const char *name = json_object_iter_peek_name(&it);
                            const char *value = json_object_get_string(obj_val);

                            // Copy alphabet
                            if (strcasecmp(name, "alphabet") == 0)
                            {
                                dict.alphabet = malloc(strlen(value) + 1);
                                if (dict.alphabet == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.alphabet, value);
                            }
                            // Copy id
                            else if (strcasecmp(name, "id") == 0)
                            {
                                dict.id = malloc(strlen(value) + 1);
                                if (dict.id == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.id, value);
                            }
                            // Copy info
                            else if (strcasecmp(name, "info") == 0)
                            {
                                dict.info = malloc(strlen(value) + 1);
                                if (dict.info == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.info, value);
                            }
                            // Copy long name
                            else if (strcasecmp(name, "long") == 0)
                            {
                                dict.long_name = malloc(strlen(value) + 1);
                                if (dict.long_name == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.long_name, value);
                            }
                            // Copy short name
                            else if (strcasecmp(name, "short") == 0)
                            {
                                dict.short_name = malloc(strlen(value) + 1);
                                if (dict.short_name == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.short_name, value);
                            }
                            // Copy url
                            else if (strcasecmp(name, "url") == 0)
                            {
                                dict.url = malloc(strlen(value) + 1);
                                if (dict.url == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.url, value);
                            }
                        }
                        // move to next object
                        json_object_iter_next(&it);
                    }
                }
                // insert the new dict into the array.
                insert_wordbook_dictionary(dict_array, dict);
                // free memory used by temp dict
                wordbook_dictionary_free_props(dict);
            }
        }
        // free the json object
        json_object_put(json_obj);
        // free the downloaded json.
        free(json_result.ptr);
    }
    return dict_array;
}

 // download all available dictionaries from wordbook.cjpg.app. 
curl_download_result wordbook_get_dictionaries_json()
{
    return wordbook_perform_http_get(API_BASE_URL API_PATH_DICTIONARIES);
}

/*****************************
 * dictionary array methods!
 *****************************/

 // Insert a 'wordbook_dictionary' into an wordbook_array_dictionary
void insert_wordbook_dictionary(wordbook_array_dictionary_t dict_array_struct_ptr, struct wordbook_dictionary dict)
{
    // increment the size, if needed.
    if (dict_array_struct_ptr->count == dict_array_struct_ptr->size)
    {

        dict_array_struct_ptr->size *= 2;

        wordbook_dictionary_t tmp = (wordbook_dictionary_t)realloc(
            dict_array_struct_ptr->dicts,
            dict_array_struct_ptr->size * sizeof(struct wordbook_dictionary)
        );

        if (tmp == NULL)
        {
            fprintf(stderr, "realloc() failed in insert_wordbook_dictionary\n");
            exit(EXIT_FAILURE);
        }

        dict_array_struct_ptr->dicts = tmp;
        // Initialize the last/new elements of the reallocated array
        for (size_t i = dict_array_struct_ptr->count; i < dict_array_struct_ptr->size; i++)
        {
            memset(&dict_array_struct_ptr->dicts[i], 0, sizeof(struct wordbook_dictionary));
        }
    }

    // simplify the code, by collecting the index.
    size_t index = dict_array_struct_ptr->count;

    // Copy the alphabet.
    size_t len = strlen(dict.alphabet) + 1;
    dict_array_struct_ptr->dicts[index].alphabet = malloc(len);
    if (dict_array_struct_ptr->dicts[index].alphabet != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].alphabet, dict.alphabet);
    }

    // Copy the id.
    len = strlen(dict.id) + 1;
    dict_array_struct_ptr->dicts[index].id = malloc(len);
    if (dict_array_struct_ptr->dicts[index].id != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].id, dict.id);
    }

    // Copy the info.
    len = strlen(dict.info) + 1;
    dict_array_struct_ptr->dicts[index].info = malloc(len);
    if (dict_array_struct_ptr->dicts[index].info != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].info, dict.info);
    }

    // Copy the long_name.
    len = strlen(dict.long_name) + 1;
    dict_array_struct_ptr->dicts[index].long_name = malloc(len);
    if (dict_array_struct_ptr->dicts[index].long_name != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].long_name, dict.long_name);
    }

    // Copy the short_name.
    len = strlen(dict.short_name) + 1;
    dict_array_struct_ptr->dicts[index].short_name = malloc(len);
    if (dict_array_struct_ptr->dicts[index].short_name != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].short_name, dict.short_name);
    }

    // Copy the url.
    len = strlen(dict.url) + 1;
    dict_array_struct_ptr->dicts[index].url = malloc(len);
    if (dict_array_struct_ptr->dicts[index].url != NULL)
    {
        strcpy(dict_array_struct_ptr->dicts[index].url, dict.url);
    }

    dict_array_struct_ptr->count++;
}


// initialize the array struct for 'wordbook_dictionary' eith a given size!
void initialize_array_wordbook_dictionary(wordbook_array_dictionary_t dict_array_struct_ptr, size_t initial_size)
{
    // Allocate initial space
    dict_array_struct_ptr->dicts = (wordbook_dictionary_t)malloc(initial_size * sizeof(struct wordbook_dictionary));
    dict_array_struct_ptr->count = 0;
    dict_array_struct_ptr->size = initial_size;

    // Initialize all values of the array to 0
    for (size_t i = 0; i < initial_size; i++)
    {
        // C6011
        if (dict_array_struct_ptr->dicts)
        {
            memset(&dict_array_struct_ptr->dicts[i], 0, sizeof(struct wordbook_dictionary));
        }
    }
}

// free memory used by the 'wordbook_array_dictionary'
void wordbook_array_dictionary_free(wordbook_array_dictionary_t dict_array)
{
    if (dict_array != NULL)
    {
        if (dict_array->dicts != NULL)
        {
            for (size_t i = 0; i < dict_array->count; i++)
            {
                wordbook_dictionary_free_props(dict_array->dicts[i]);
            }
            free(dict_array->dicts);
        }
        free(dict_array);
    }
}

// free memory used by props in 'wordbook_dictionary'
// make sure all props are NULL or a valid pointer.
void wordbook_dictionary_free_props(struct wordbook_dictionary dict)
{
    if (dict.alphabet)
    {
        free(dict.alphabet);
    }
    if (dict.id)
    {
        free(dict.id);
    }
    if (dict.info)
    {
        free(dict.info);
    }
    if (dict.long_name)
    {
        free(dict.long_name);
    }
    if (dict.short_name)
    {
        free(dict.short_name);
    }
    if (dict.url)
    {
        free(dict.url);
    }
}


// free memory used by 'wordbook_dictionary'
void wordbook_dictionary_free(wordbook_dictionary_t dict_struct_ptr)
{
    wordbook_dictionary_free_props(*dict_struct_ptr);
    if (dict_struct_ptr != NULL)
    {
        free(dict_struct_ptr);
    }
}