#ifndef _PARSER_H
#define _PARSER_H

#include "jsmn.h"
#include "stdlib.h"
#include "stm32f4xx_hal.h"

typedef enum {
    JSON_INT,
    JSON_FLOAT,
    JSON_STRING,
    JSON_OBJECT,
    JSON_ARRAY_INT,
    JSON_ARRAY_FLOAT,
    JSON_ARRAY_STRING,
    JSON_ARRAY_OBJECT
} json_type;

typedef struct {
    char name[15];
    json_type type;
    void *data;
} JsonData;

int SS_json_parse_string(char *json, jsmntok_t *tok, char *name, char *value);
int8_t SS_json_parse_int(char *json, jsmntok_t *tok, char *name, int *value);
int8_t SS_json_parse_float(char *json, jsmntok_t *tok, char *name, float *value);
int SS_json_parse_object(char *json, jsmntok_t *tok, char *name, jsmntok_t **out);
int SS_json_parse_array_int(char *json, jsmntok_t *tok, char *name, int *array);
int SS_json_parse_array_float(char *json, jsmntok_t *tok, char *name, float *array);
int SS_json_parse_array_string(char *json, jsmntok_t *tok, char *name, char array[][10]);
int SS_json_parse_array_object(char *json, jsmntok_t *tok, char *name, jsmntok_t **array);
int SS_json_get_object_length(char *json, jsmntok_t *tok);
int8_t SS_json_parse_data_single(JsonData *i, char *json, jsmntok_t *tok);
void SS_json_parse_data(JsonData *it, int length, char *json, jsmntok_t *tok);

#endif
