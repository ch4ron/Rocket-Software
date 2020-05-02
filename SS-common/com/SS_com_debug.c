/*
 * SS_com_debug.c
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#include "SS_com_debug.h"

#include "string.h"
#ifdef SS_USE_GRAZYNA
#include "SS_grazyna.h"
#endif

void print_board(uint32_t board, char *color) {
    switch(board) {
        case COM_GRAZYNA_ID:
            SS_print("\x01b[48;5;9mGrazyna");
            break;
        case COM_STASZEK_ID:
            SS_print("\x01b[48;5;6mStaszek");
            break;
        case COM_RADEK_ID:
            SS_print("\x01b[48;5;2mRadek");
            break;
        case COM_CZAPLA_ID:
            SS_print("\x01b[48;5;3mCzapla");
            break;
        case COM_PAUEK_ID:
            SS_print("\x01b[48;5;4mPauek");
            break;
        case COM_KROMEK_ID:
            SS_print("\x01b[48;5;5mKromek");
            break;
        default:
            SS_print("\x01b[41mid: %lu", board);
    }
    SS_print("%s", color);
}

void print_action(uint32_t action) {
    switch (action) {
        case COM_FEED:
            SS_print("feed");
            break;
        case COM_SERVICE:
            SS_print("serv");
            break;
        case COM_ACK:
            SS_print("ack");
            break;
        case COM_NACK:
            SS_print("nack");
            break;
        case COM_HEARTBEAT:
            SS_print("beat");
            break;
        case COM_REQUEST:
            SS_print("req");
            break;
        case COM_RESPONSE:
            SS_print("res");
            break;
        default:
            SS_print("unknown");
    }
}

void print_payload(ComFrame *frame) {
    if(frame->data_type == NO_DATA) return;
    SS_print("\t Data: ");
    switch(frame->data_type) {
        case NO_DATA:
            break;
        case UINT8:
        case UINT16:
        case UINT32:
            SS_print("%u", (uint16_t) frame->payload);
            break;
        case INT8:
        case INT16:
        case INT32:
            SS_print("%d", (uint16_t) frame->payload);
            break;
        case FLOAT:;
            float buf;
            memcpy(&buf, &frame->payload, sizeof(float));
            SS_print("%f", buf);
            break;
    }
}

void SS_com_debug_print_frame(ComFrame *frame, char *title, char *color) {
#ifdef SS_COM_DEBUG
    SS_print("%s%s\t", color, title);
    SS_print("From: ");
    print_board(frame->source, color);
    SS_print("\t To: ");
    print_board(frame->destination, color);
    SS_print("\t Priority: %u ", frame->priority);
    SS_print("\t action: ");
    print_action(frame->action);
    SS_print("\t device: 0x%02x", frame->device);
    SS_print("\t id: 0x%02x", frame->id);
    SS_print("\t operation: 0x%02x%s", frame->operation, color);
    print_payload(frame);
    SS_print("\r\n");
#endif
}

void SS_can_print_message_received(ComFrame *frame) {
    char color[] = "\x01b[48;5;238m";
    char title[] = "Can Rec:";
    SS_com_debug_print_frame(frame, title, color);
}

void SS_can_print_message_sent(ComFrame *frame) {
    char color[] = "\x01b[48;5;234m";
    char title[] = "Can Sent:";
    SS_com_debug_print_frame(frame, title, color);
}

void SS_grazyna_print_message_received(ComFrame *frame) {
    char color[] = "\x01b[48;5;240m";
    char title[] = "Gr  Rec:";
    SS_com_debug_print_frame(frame, title, color);
}

void SS_grazyna_print_message_sent(ComFrame *frame) {
    char color[] = "\x01b[48;5;236m";
    char title[] = "Gr  Sent:";
    SS_com_debug_print_frame(frame, title, color);
}
void SS_com_print_message_error(ComFrame *frame, char *error) {
    char color[] = "\x01b[41m";
    SS_com_debug_print_frame(frame, error, color);
}

#ifdef SS_COM_DEBUG_HEX
#ifdef SS_USE_GRAZYNA
static void SS_grazyna_print_hex(GrazynaFrame *frame) {
    for(uint8_t i = 0; i < sizeof(GrazynaFrame); i++) {
        SS_print("0x%02x ", ((uint8_t *) frame)[i]);
    }
    SS_print("\r\n");
}

#endif

void print_binary(uint32_t number) {
    uint8_t bin[32];
    uint8_t i = 0;
    while(number > 0) {
        bin[++i] = number % 2;
        number /= 2;
    }
    SS_print("\r\n0b");
    SS_print("%0*d", 29 - i, 0);
    for(; i > 0; i--) {
        if(!(i % 8))
            SS_print(" ");
        SS_print("%d", bin[i]);
    }
}

static void SS_com_print_hex(ComFrame *frame) {
    SS_print("\r\n");
    for(uint8_t i = 0; i < sizeof(ComFrame); i++) {
        SS_print("0x%02x ", ((uint8_t *) frame)[i]);
    }
}
#endif
