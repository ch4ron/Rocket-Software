/*
 * SS_settings.c
 *
 *  Created on: Feb 5, 2020
 *      Author: maciek
 */

#include "settings_json.h"
#include "SS_settings.h"
#include "SS_json_parser.h"
#include "string.h"
#include "SS_servos.h"
#include "stdio.h"

static jsmn_parser p;
static jsmntok_t t[128];
static int r;

void SS_json_parse(char *json) {
    jsmn_init(&p);
    r = jsmn_parse(&p, json, strlen(json), t, sizeof(t) / sizeof(t[0]));
    if (r < 0) {
        /* Replace with assertion */
        printf("Failed to parse JSON");
    }
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        /* Replace with assertion */
        printf("Object expected");
    }
}

void SS_settings_read_json(char* json) {
    jsmntok_t *servos_tok[8];
    int boardID;
    JsonData data[] = {
        {
            .name = "servos",
            .type = JSON_ARRAY_OBJECT,
            .data = &servos_tok
        },
//        {
//            .name = "measurements",
//            .type = JSON_ARRAY_OBJECT,
//            .data = &measurements_tok
//        },
        {
            .name = "boardID",
            .type = JSON_INT,
            .data = &boardID
        },
    };
    SS_json_parse(json);
    SS_json_parse_data(data, sizeof(data)/sizeof(data[0]), json, t);
    SS_servos_read_json(json, servos_tok);
}
