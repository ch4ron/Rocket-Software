#include "settings_json.h"
#include "stdio.h"
#include "string.h"
#include "SS_json_parser.h"
#include "stdint.h"

static int8_t jsoneq(char *json, jsmntok_t *tok, char *s) {
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

void print_jsmntok(char *json, jsmntok_t *tok) {
    if(tok->type == JSMN_OBJECT) 
        printf("%.*s\r\n", tok->end - tok->start, json + tok->start);
    else if(tok->type == JSMN_ARRAY)
        printf("%.*s\r\n", tok->end - tok->start, json + tok->start);
    else 
        printf("\r\n%.*s: %.*s\r\n", tok->end - tok->start, json + tok->start,
         tok[1].end - tok[1].start, json + tok[1].start);
}

static int parse_raw(char *json, jsmntok_t *tok, void *str) {
    memset(str, 0, tok->end - tok->start + 1);
    memcpy(str, json + tok->start, tok->end - tok->start);
    return tok->end - tok->start;
}

static int parse(char *json, jsmntok_t *tok, char *name, void *str) {
  if(jsoneq(json, tok, name) == 0) {
    return parse_raw(json, &tok[1], str);
  }
  return -1;
}

//returns string length or -1 in case of error
int SS_json_parse_string(char *json, jsmntok_t *tok, char *name, char *value) {
    if(tok[1].type != JSMN_STRING) return -1;
    return parse(json, tok, name, value);
}

int8_t SS_json_parse_int(char *json, jsmntok_t *tok, char *name, int *value) {
    char str[10]; //max 9 characters
    if(tok[1].type != JSMN_PRIMITIVE) return -1;
    if(parse(json, tok, name, str) > 0) {
        *value = atoi(str);
        return 0;
    }
    return -1;
}

int8_t SS_json_parse_float(char *json, jsmntok_t *tok, char *name, float *value) {
    char str[10]; //max 9 characters
    if(tok[1].type != JSMN_PRIMITIVE) return -1;
    if(parse(json, tok, name, str) > 0) {
      *value = (float) atof(str);
      return 0;
    }
    return -1;
}

//returns number of tokens for object (including nested ones) or -1 in case of error
int SS_json_parse_object(char *json, jsmntok_t *tok, char *name, jsmntok_t **out) {
    if(tok[1].type != JSMN_OBJECT) return -1;
    if(jsoneq(json, tok, name) == 0) {
        *out = &tok[1];
        return SS_json_get_object_length(json, tok) - 1;
    } 
    return -1;
}

int SS_json_parse_array_int(char *json, jsmntok_t *tok, char *name, int *array) {
    if(tok[1].type != JSMN_ARRAY) return -1;
    char buf[10];
    if(jsoneq(json, tok, name) == 0) {
        for(int i = 0; i < tok[1].size; i++) {
            parse_raw(json, &tok[i + 2], buf);
            array[i] = atoi(buf);
        }
        return 0;
    } 
    return -1;
}

int SS_json_parse_array_float(char *json, jsmntok_t *tok, char *name, float *array) {
    if(tok[1].type != JSMN_ARRAY) return -1;
    char buf[10];
    if(jsoneq(json, tok, name) == 0) {
        for(int i = 0; i < tok[1].size; i++) {
            parse_raw(json, &tok[i + 2], buf);
            array[i] = atof(buf);
        }
        return 0;
    } 
    return -1;
}

int SS_json_parse_array_string(char *json, jsmntok_t *tok, char *name, char array[][10]) {
    if(tok[1].type != JSMN_ARRAY) return -1;
    char buf[10];
    if(jsoneq(json, tok, name) == 0) {
        for(int i = 0; i < tok[1].size; i++) {
            parse_raw(json, &tok[i + 2], buf);
            strcpy(array[i], buf);
        }
        return 0;
    } 
    return -1;
}

int SS_json_parse_array_object(char *json, jsmntok_t *tok, char *name, jsmntok_t **array) {
    if(tok[1].type != JSMN_ARRAY) return -1;
    int length = 0;
    if(jsoneq(json, tok, name) == 0) {
        for(int i = 0; i < tok[1].size; i++) {
            array[i] = &tok[length + 2];
            length += SS_json_get_object_length(json, &tok[length + 2]);
        }
        return length;
    }
    return -1;

}

int SS_json_get_object_length(char *json, jsmntok_t *tok) {
    int length = 1;
    int j = 0;
    if(tok->type == JSMN_OBJECT) {
        for(int i = 0; i < tok->size; i++) {
            int len = SS_json_get_object_length(json, &tok[j+1]);
            j += len;
            length += len;
        }
        return length;
    }
    if(tok->type == JSMN_ARRAY && tok[1].type == JSMN_OBJECT) {
        for(int i = 0; i < tok->size; i++) {
            int len = SS_json_get_object_length(json, &tok[j+1]);
            j += len;
            length += len;
        }
        return length;
    }
    if(tok->type == JSMN_ARRAY) {
        return tok->size + 1;
    }
    if(tok[1].type == JSMN_OBJECT || tok[1].type == JSMN_ARRAY) {
        return SS_json_get_object_length(json, &tok[1]) + 1;
    }
    return 2;
}

int8_t SS_json_parse_data_single(JsonData *i, char *json, jsmntok_t *tok) {
    switch(i->type) {
        case JSON_INT:
            return SS_json_parse_int(json, tok, i->name, (int*) i->data);
        case JSON_FLOAT:
            return SS_json_parse_float(json, tok, i->name, (float*) i->data);
        case JSON_STRING:
            return SS_json_parse_string(json, tok, i->name, (char*) i->data);
        case JSON_OBJECT:
            return SS_json_parse_object(json, tok, i->name, (jsmntok_t**) i->data);
        case JSON_ARRAY_INT:
            return SS_json_parse_array_int(json, tok, i->name, (int*) i->data);
        case JSON_ARRAY_FLOAT:
            return SS_json_parse_array_float(json, tok, i->name, (float*) i->data);
        case JSON_ARRAY_STRING:
            return SS_json_parse_array_string(json, tok, i->name, (char (*)[10]) i->data);
        case JSON_ARRAY_OBJECT:
            return SS_json_parse_array_object(json, tok, i->name, (jsmntok_t**) i->data);
    }   
    return -1;
}

//parses json data to a list of interfaces
void SS_json_parse_data(JsonData *it, int length, char *json, jsmntok_t *tok) {
    int size = SS_json_get_object_length(json, tok) - 1;
    for(int i = 1; i < size; i += SS_json_get_object_length(json, &tok[i])) {
        for(int j = 0; j < length; j++) {
            SS_json_parse_data_single(&it[j], json, &tok[i]);
        }
    }
}

static int save_raw(char *json, uint16_t json_length, jsmntok_t *tok, char *str) {
	char buff[1024];
	uint16_t length = strlen(str);
	if(json_length - (tok->end - tok->start) + length >= 1024) {
	    /* TODO - test and replace with assertion */
	    printf("Not enough space allocated for json\r\n");
	    return -1;
	}
	if(length == tok->end - tok->start) {
	    memcpy(json + tok->start, str, length);
	} else {
	    memcpy(buff, json + tok->end, json_length - tok->end);
	    memcpy(json + tok->start, str, length);
	    memcpy(json + tok->start + length, buff, json_length - tok->end);
	}
    return tok->end - tok->start;
}

static int save(char *json, uint16_t json_length, jsmntok_t *tok, char *name, char *str) {
  if(jsoneq(json, tok, name) == 0) {
    return save_raw(json, json_length, &tok[1], str);
  }
  return -1;
}

int SS_json_save_string(char *json, uint16_t json_length, jsmntok_t *tok, char *name, char* value) {
    if(tok[1].type != JSMN_STRING) return -1;
    return save(json, json_length, tok, name, value);
}

int8_t SS_json_save_int(char *json, uint16_t json_length, jsmntok_t *tok, char *name, int value) {
    char str[10]; //max 9 characters
    if(tok[1].type != JSMN_PRIMITIVE) return -1;
    sprintf(str, "%d", value);
    return save(json, json_length, tok, name, str);
}

int8_t SS_json_save_float(char *json, uint16_t json_length, jsmntok_t *tok, char *name, float value) {
    char str[10]; //max 9 characters
    if(tok[1].type != JSMN_PRIMITIVE) return -1;
    sprintf(str, "%f", value);
    return save(json, json_length, tok, name, str);
}

int8_t SS_json_save_data_single(JsonData *i, char *json, uint16_t json_length, jsmntok_t *tok) {
    switch(i->type) {
        case JSON_INT:
            return SS_json_save_int(json, json_length, tok, i->name, *((int*) i->data));
        case JSON_FLOAT:
            return SS_json_save_float(json, json_length, tok, i->name, *((float*) i->data));
        case JSON_STRING:
            return SS_json_save_string(json, json_length, tok, i->name, (char*) i->data);
        default:
			return -1;
    }
}

void SS_json_save_data(JsonData *it, char *json, uint16_t json_length, jsmntok_t *tok) {
    int size = SS_json_get_object_length(json, tok) - 1;
    for(int i = 1; i < size; i += SS_json_get_object_length(json, &tok[i])) {
		SS_json_save_data_single(it, json, json_length, &tok[i]);
    }
}
