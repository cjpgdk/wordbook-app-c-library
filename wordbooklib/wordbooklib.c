#define CURL_STATICLIB
#define _CRT_SECURE_NO_WARNINGS
#include "wordbooklib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

// get all available dictionaries
wordbook_array_dictionary *wordbook_get_dictionaries()
{
    // get the raw json data
    curl_download_result json_result;
    json_result = wordbook_get_dictionaries_json();

    // declare the array to hold the dictionaries
    wordbook_array_dictionary *dict_array = (wordbook_array_dictionary *)malloc(sizeof(wordbook_array_dictionary));

    int dictionaries_count = 0;
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
            for (int i = 0; i < dictionaries_count; i++)
            {
                /*
                 * Finaly we got the actual dictionary object!
                 */
                 // get the array item at index 'i'
                json_object *obj = json_object_array_get_idx(json_obj, i);

                // create a temperary dict.
                wordbook_dictionary dict = (wordbook_dictionary){ NULL,  NULL, NULL, NULL, NULL, NULL };

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
                            char *name = json_object_iter_peek_name(&it);
                            char *value = json_object_get_string(obj_val);

                            // Copy alphabet
                            if (_strcmpi(name, "alphabet") == 0)
                            {
                                dict.alphabet = malloc(strlen(value) + 1);
                                if (dict.alphabet == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.alphabet, value);
                            }
                            // Copy id
                            else if (_strcmpi(name, "id") == 0)
                            {
                                dict.id = malloc(strlen(value) + 1);
                                if (dict.id == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.id, value);
                            }
                            // Copy info
                            else if (_strcmpi(name, "info") == 0)
                            {
                                dict.info = malloc(strlen(value) + 1);
                                if (dict.info == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.info, value);
                            }
                            // Copy long name
                            else if (_strcmpi(name, "long") == 0)
                            {
                                dict.long_name = malloc(strlen(value) + 1);
                                if (dict.long_name == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.long_name, value);
                            }
                            // Copy short name
                            else if (_strcmpi(name, "short") == 0)
                            {
                                dict.short_name = malloc(strlen(value) + 1);
                                if (dict.short_name == NULL)
                                {
                                    continue;
                                }
                                strcpy(dict.short_name, value);
                            }
                            // Copy url
                            else if (_strcmpi(name, "url") == 0)
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

// Insert a 'wordbook_dictionary' into an wordbook_array_dictionary
void insert_wordbook_dictionary(wordbook_array_dictionary *dict_array, wordbook_dictionary dict)
{
    // increment the size, if needed.
    if (dict_array->count == dict_array->size)
    {
        // only increment by 5, no need to take more than needed.
        dict_array->size += 5;
        dict_array->dicts = (wordbook_dictionary *)realloc(dict_array->dicts, dict_array->size * sizeof(wordbook_dictionary));
        if (dict_array->dicts == NULL)
        {
            fprintf(stderr, "realloc() failed in insert_wordbook_dictionary\n");
            exit(EXIT_FAILURE);
        }
        // Initialize the last/new elements of the reallocated array
        for (unsigned int i = dict_array->count; i < dict_array->size; i++)
        {
            memset(&dict_array->dicts[i], 0, sizeof(wordbook_dictionary));
        }
    }

    // simplify the code, by collecting the index.
    size_t index = dict_array->count;

    // Copy the alphabet.
    size_t len = strlen(dict.alphabet) + 1;
    dict_array->dicts[index].alphabet = malloc(len);
    if (dict_array->dicts[index].alphabet != NULL)
    {
        strcpy(dict_array->dicts[index].alphabet, dict.alphabet);
    }

    // Copy the id.
    len = strlen(dict.id) + 1;
    dict_array->dicts[index].id = malloc(len);
    if (dict_array->dicts[index].id != NULL)
    {
        strcpy(dict_array->dicts[index].id, dict.id);
    }

    // Copy the info.
    len = strlen(dict.info) + 1;
    dict_array->dicts[index].info = malloc(len);
    if (dict_array->dicts[index].info != NULL)
    {
        strcpy(dict_array->dicts[index].info, dict.info);
    }

    // Copy the long_name.
    len = strlen(dict.long_name) + 1;
    dict_array->dicts[index].long_name = malloc(len);
    if (dict_array->dicts[index].long_name != NULL)
    {
        strcpy(dict_array->dicts[index].long_name, dict.long_name);
    }

    // Copy the short_name.
    len = strlen(dict.short_name) + 1;
    dict_array->dicts[index].short_name = malloc(len);
    if (dict_array->dicts[index].short_name != NULL)
    {
        strcpy(dict_array->dicts[index].short_name, dict.short_name);
    }

    // Copy the url.
    len = strlen(dict.url) + 1;
    dict_array->dicts[index].url = malloc(len);
    if (dict_array->dicts[index].url != NULL)
    {
        strcpy(dict_array->dicts[index].url, dict.url);
    }

    dict_array->count++;
}

// initialize the array struct for 'wordbook_dictionary' eith a given size!
void initialize_array_wordbook_dictionary(wordbook_array_dictionary *dict_array, size_t initial_size)
{
    // Allocate initial space
    dict_array->dicts = (wordbook_dictionary *)malloc(initial_size * sizeof(wordbook_dictionary));
    dict_array->count = 0;
    dict_array->size = initial_size;

    // Initialize all values of the array to 0
    for (int i = 0; i < initial_size; i++)
    {
        memset(&dict_array->dicts[i], 0, sizeof(wordbook_dictionary));
    }
}

// free memory used by the 'wordbook_array_dictionary'
void wordbook_array_dictionary_free(wordbook_array_dictionary *dict_array)
{
    if (dict_array != NULL)
    {
        if (dict_array->dicts != NULL)
        {
            for (int i = 0; i < dict_array->count; i++)
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
void wordbook_dictionary_free_props(wordbook_dictionary dict)
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
void wordbook_dictionary_free(wordbook_dictionary *dict)
{
    wordbook_dictionary_free_props(*dict);
    if (dict != NULL)
    {
        free(dict);
    }
}


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

// function used to initialize the curl_download_result
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