#include <stdio.h>
#include"wordbooklib.h"
#include <json-c/json.h>

int main(int argc, char *argv[])
{
    struct curl_download_result json_result;
    json_result = wordbook_get_dictionaries_json();

    if (json_result.ptr != NULL)
    {
        struct json_object *json_obj;
        json_obj = json_tokener_parse(json_result.ptr);
        printf("Result after parse:\n%s\n\n", json_object_to_json_string(json_obj));
        free(json_result.ptr);
    }

    return 0;
}