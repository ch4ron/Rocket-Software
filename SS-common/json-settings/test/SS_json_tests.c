#include <string.h>
#include "unity_fixture.h"
#include "jsmn.h"
#include "test_json.h"
#include "SS_json_parser.h"

/* TODO write tests for saving data */

static jsmn_parser p;
static jsmntok_t t[128];
static int r;

TEST_GROUP(parser);

TEST_GROUP_RUNNER(parser) {
    RUN_TEST_CASE(parser, SS_json_parse_string);
    RUN_TEST_CASE(parser, SS_json_parse_int);
    RUN_TEST_CASE(parser, SS_json_parse_float);
    RUN_TEST_CASE(parser, parse_nested);
    RUN_TEST_CASE(parser, SS_json_parse_array_int);
    RUN_TEST_CASE(parser, SS_json_parse_array_float);
    RUN_TEST_CASE(parser, SS_json_parse_array_string);
    RUN_TEST_CASE(parser, parse_deep_nested);
    RUN_TEST_CASE(parser, object_length);
    RUN_TEST_CASE(parser, json_length);
    RUN_TEST_CASE(parser, parse_data_int);
    RUN_TEST_CASE(parser, parse_data_float);
    RUN_TEST_CASE(parser, parse_data_string);
    RUN_TEST_CASE(parser, parse_data_array);
    RUN_TEST_CASE(parser, parse_data_object);
    RUN_TEST_CASE(parser, parse_data_nested);
    RUN_TEST_CASE(parser, parse_data_array_int);
    RUN_TEST_CASE(parser, parse_data_array_float);
    RUN_TEST_CASE(parser, parse_data_array_string);
    RUN_TEST_CASE(parser, parse_data_array_object);
}

TEST_SETUP(parser) {
    jsmn_init(&p);
    r = jsmn_parse(&p, test_json, strlen(test_json), t, sizeof(t) / sizeof(t[0]));
    if (r < 0) {
        TEST_FAIL_MESSAGE("Failed ot parse JSON");
    }
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        TEST_FAIL_MESSAGE("Object expected");
    }

}

TEST_TEAR_DOWN(parser) {}


void run_all_tests() {
    RUN_TEST_GROUP(parser);
}

#ifdef TESTS_ON_PC
int main(int argc, const char *argv[]) {
    return UnityMain(argc, argv, run_all_tests);
}
#endif

TEST(parser, SS_json_parse_string) {
    char value[15];
    for(int i = 1; i < r; i++) {
        SS_json_parse_string(test_json, &t[i], "board", value);
    }
    TEST_ASSERT_EQUAL_STRING("Kromek", value);
}

TEST(parser, SS_json_parse_int) {
    int value;
    for(int i = 1; i < r;  i++) {
        SS_json_parse_int(test_json, &t[i], "boardID", &value);
    }
    TEST_ASSERT_EQUAL_INT(3, value);
}

TEST(parser, SS_json_parse_float) {
    float value;
    for(int i = 1; i < r; i++) {
        SS_json_parse_float(test_json, &t[i], "value", &value);
    }
    TEST_ASSERT_EQUAL_FLOAT(145.2134, value);
}

TEST(parser, parse_nested) {
    jsmntok_t *tok;
    int size = 0, value;
    for(int i = 1; i < r; i += SS_json_get_object_length(test_json, &t[i])) {
        int tmp = SS_json_parse_object(test_json, &t[i], "parameters", &tok);
        if(tmp > 0) size = tmp;
    }
    TEST_ASSERT_EQUAL_INT(9, size);
    for(int i = 1; i < size; i+= SS_json_get_object_length(test_json, &tok[i])) {
        SS_json_parse_int(test_json, &tok[i], "y", &value);
    }
    TEST_ASSERT_EQUAL_INT(11, value);
}

TEST(parser, parse_deep_nested) {
    jsmntok_t *tok, *tok2;
    int size = 0, size2 = 0, value;
    for(int i = 1; i < r; i += SS_json_get_object_length(test_json, &t[i])) {
        int tmp = SS_json_parse_object(test_json, &t[i], "parameters", &tok);
        if(tmp > 0) size = tmp;
    }
    TEST_ASSERT_EQUAL_INT(9, size);
    for(int i = 1; i < size; i += SS_json_get_object_length(test_json, &tok[i])) {
        int tmp = SS_json_parse_object(test_json, &tok[i], "z", &tok2);
        if(tmp > 0) size2 = tmp;
    }
    TEST_ASSERT_EQUAL_INT(3, size2);
    for(int i = 1; i < size2; i += SS_json_get_object_length(test_json, &tok2[i])) {
        SS_json_parse_int(test_json, &tok2[i], "a", &value);
    }
    TEST_ASSERT_EQUAL_INT(17, value);
}

TEST(parser, SS_json_parse_array_int) {
    int arr[4];
    for(int i = 1; i < r; i++) {
        SS_json_parse_array_int(test_json, &t[i], "array", arr);
    }
    int expected[] = {1, 2, 3, 4};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, arr, 4);
}

TEST(parser, SS_json_parse_array_float) {
    float arr[4];
    for(int i = 1; i < r; i++) {
        SS_json_parse_array_float(test_json, &t[i], "array_float", arr);
    }
    float expected[] = {1.1, 2.1, 3.1, 4.1};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, arr, 4);
}

TEST(parser, SS_json_parse_array_string) {
    char arr[4][10];
    for(int i = 1; i < r; i++) {
        SS_json_parse_array_string(test_json, &t[i], "array_string", arr);
    }
    char expected[3][4] = {{'a', 'b', 'c', 0}, {'x', 'y', 'z', 0}, {'c', 0}};
    for(int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_STRING(expected[i], arr[i]);
    }
}

TEST(parser, object_length) {
    TEST_ASSERT_EQUAL_INT(10, SS_json_get_object_length(test_json, &t[1]));
}

TEST(parser, json_length) {
    TEST_ASSERT_EQUAL_INT(45, SS_json_get_object_length(test_json, &t[0]));
}

TEST(parser, parse_data_int) {
    int value;
    JsonData it = {
        .name = "boardID",
        .type = JSON_INT,
        .data = &value
    };
    for(int i = 1; i < r;  i += SS_json_get_object_length(test_json, &t[i])) {
        SS_json_parse_data_single(&it, test_json, &t[i]);
    }
    TEST_ASSERT_EQUAL_INT(3, value);
    TEST_ASSERT_EQUAL_INT(3, *((int*) it.data));
}

TEST(parser, parse_data_float) {
    float value;
    JsonData it[1] = {
        {
            .name = "value",
            .type = JSON_FLOAT,
            .data = &value
        }
    };
    SS_json_parse_data(it, 1, test_json, t);
    TEST_ASSERT_EQUAL_FLOAT(145.2134, value);
}

TEST(parser, parse_data_string) {
    char value[10];
    JsonData it[1] = {
        {
            .name = "board",
            .type = JSON_STRING,
            .data = value
        }
    };
    SS_json_parse_data(it, 1, test_json, t);
    TEST_ASSERT_EQUAL_STRING("Kromek", value);
}

TEST(parser, parse_data_object) {
    jsmntok_t *tok;
    JsonData it[1] = {
        {
            .name = "parameters",
            .type = JSON_OBJECT,
            .data = &tok
        }
    };
    SS_json_parse_data(it, 1, test_json, t);
    TEST_ASSERT_EQUAL_INT(&t[2], tok);
}

TEST(parser, parse_data_array) {
    int value;
    float fvalue;
    JsonData it[2] = {
        {
            .name = "boardID",
            .type = JSON_INT,
            .data = &value
        },
        {
            .name = "value",
            .type = JSON_FLOAT,
            .data = &fvalue
        }
    };
    SS_json_parse_data(it, 2, test_json, t);
    TEST_ASSERT_EQUAL_INT(3, value);
    TEST_ASSERT_EQUAL_FLOAT(145.2134, fvalue);
}

TEST(parser, parse_data_nested) {
    jsmntok_t *tok;
    int value;
    JsonData it[2] = {
        {
            .name = "parameters",
            .type = JSON_OBJECT,
            .data = &tok
        },
        {
            .name = "y",
            .type = JSON_INT,
            .data = &value
        }
    };
    SS_json_parse_data(it, 1, test_json, t);
    TEST_ASSERT_EQUAL_INT(&t[2], tok);
    SS_json_parse_data(it, 2, test_json, tok);
    TEST_ASSERT_EQUAL_INT(11, value);
}

TEST(parser, parse_data_array_int) {
    int arr[4];
    JsonData it[1] = {
        {
         .name = "array",
         .type = JSON_ARRAY_INT,
         .data = arr
     }
    };
    SS_json_parse_data(it, 1, test_json, t);
    int expected[] = {1, 2, 3, 4};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected, arr, 4);
}

TEST(parser, parse_data_array_float) {
    float arr[4];
    JsonData it[1] = {
        {
         .name = "array_float",
         .type = JSON_ARRAY_FLOAT,
         .data = arr
     }
    };
    SS_json_parse_data(it, 1, test_json, t);
    float expected[] = {1.1, 2.1, 3.1, 4.1};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, arr, 4);
}

TEST(parser, parse_data_array_string) {
    char arr[4][10];
    JsonData it[1] = {
        {
         .name = "array_string",
         .type = JSON_ARRAY_STRING,
         .data = arr
     }
    };
    SS_json_parse_data(it, 1, test_json, t);
    char expected[3][4] = {{'a', 'b', 'c', 0}, {'x', 'y', 'z', 0}, {'c', 0}};
    for(int i = 0; i < 3; i++) {
        TEST_ASSERT_EQUAL_STRING(expected[i], arr[i]);
    }
}

TEST(parser, parse_data_array_object) {
    jsmntok_t *tok[3];
    JsonData it[1] = {
        {
         .name = "array_object",
         .type = JSON_ARRAY_OBJECT,
         .data = &tok
        }
    };
    int a, b ,c;
    JsonData it1[] = {
        {
         .name = "a",
         .type = JSON_INT,
         .data = &a
        }
    };
    JsonData it2[] = {
        {
         .name = "b",
         .type = JSON_INT,
         .data = &b
        }
    };
    JsonData it3[] = {
        {
         .name = "c",
         .type = JSON_INT,
         .data = &c
        }
    };
    SS_json_parse_data(it, 1, test_json, t);
    SS_json_parse_data(it1, 1, test_json, tok[0]);
    SS_json_parse_data(it2, 1, test_json, tok[1]);
    SS_json_parse_data(it3, 1, test_json, tok[2]);
    TEST_ASSERT_EQUAL(1, a);
    TEST_ASSERT_EQUAL(2, b);
    TEST_ASSERT_EQUAL(3, c);
}
