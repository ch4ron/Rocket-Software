
#include "../SS_sequence_test.c"

void tests(void) {
    RUN_TEST_GROUP(sequence);
}

int main(int argc, const char *argv[]) {
    return UnityMain(argc, argv, tests);
}
