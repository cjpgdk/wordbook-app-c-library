#ifndef WORDBOOOKLIB_H
#define WORDBOOOKLIB_H

#include <stdio.h>

// struct for downloaded data!
struct curl_download_result {
    char* ptr;
    size_t len;
};

struct curl_download_result wordbook_get_dictionaries_json();



#endif // !WORDBOOOKLIB_H