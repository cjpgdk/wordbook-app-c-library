
#include <stdio.h>
#include"wordbooklib.h"
#include <json-c/json.h>
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


int main(int argc, char *argv[])
{
#if _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    struct curl_download_result json_result;
    json_result = wordbook_get_dictionaries_json();

    if (json_result.ptr != NULL)
    {
        struct json_object *json_obj;
        json_obj = json_tokener_parse(json_result.ptr);

        printf("Result after parse:\n%s\n\n", json_object_to_json_string(json_obj));
        json_object_put(json_obj);
        free(json_result.ptr);
    }

    return 0;
}