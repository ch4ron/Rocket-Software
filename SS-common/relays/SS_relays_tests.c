//
// Created by maciek on 29.02.2020.
//

#include "SS_relays.h"
#include "SS_com_protocol.h"
#include "unity_fixture.h"

TEST_GROUP(relays);

TEST_GROUP_RUNNER(relays) {
    RUN_TEST_CASE(relays, open);
    RUN_TEST_CASE(relays, close);
    RUN_TEST_CASE(relays, status);
}


TEST_SETUP(relays) {}

TEST_TEAR_DOWN(relays) {}

extern Relay *relay_pointers[MAX_RELAY_COUNT];

static void test_relay_open(Relay *relay) {
    SS_relay_close(relay);
    ComFrameContent frame = {
            .action = COM_SERVICE,
            .device = COM_RELAY_ID,
            .id = relay->id,
            .message_type = COM_RELAY_OPEN};
    TEST_ASSERT_EQUAL(relay->state, 0);
    ComStatus res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(relay->state, 1);
}

static void test_relay_close(Relay *relay) {
    SS_relay_close(relay);
    ComFrameContent frame = {
            .action = COM_SERVICE,
            .device = COM_RELAY_ID,
            .id = relay->id,
            .message_type = COM_RELAY_OPEN};
    TEST_ASSERT_EQUAL(relay->state, 0);
    ComStatus res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(relay->state, 1);
}

static void test_relay_get_status(Relay *relay) {
    SS_relay_open(relay);
    ComFrameContent frame = {
            .action = COM_REQUEST,
            .device = COM_RELAY_ID,
            .id = relay->id,
            .message_type = COM_RELAY_STATUS};
    ComStatus res = SS_com_handle_action(&frame);
    TEST_ASSERT_EQUAL(COM_OK, res);
    TEST_ASSERT_EQUAL(1, frame.payload);
    TEST_ASSERT_EQUAL(UINT8, frame.data_type);
    SS_relay_close(relay);
}

TEST(relays, open) {
    for (uint8_t i = 0; i < MAX_RELAY_COUNT; i++) {
        test_relay_open(relay_pointers[i]);
    }
}

TEST(relays, close) {
    for (uint8_t i = 0; i < MAX_RELAY_COUNT; i++) {
        test_relay_close(relay_pointers[i]);
    }
}

TEST(relays, status) {
    for (uint8_t i = 0; i < MAX_RELAY_COUNT; i++) {
        test_relay_get_status(relay_pointers[i]);
    }
}

