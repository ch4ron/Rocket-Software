/*
 * SS_com_debug.c
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#include "SS_com_debug.h"
#include "string.h"
#include "stdio.h"

//void print_binary(uint32_t number) {
//    uint8_t bin[32];
//    uint8_t i = 0;
//    while(number > 0) {
//        bin[++i] = number%2;
//        number /= 2;
//    }
//    printf("\r\n0b");
//    printf("%0*d", 29-i, 0);
//    for(; i > 0; i--) {
//        if(!(i%8))
//            printf(" ");
//        printf("%d", bin[i]);
//    }
//}

static void SS_com_print_hex(ComFrame *frame) {
    printf("\r\n");
    for(uint8_t i = 0; i < sizeof(ComFrame); i++) {
        printf("0x%02x ", ((uint8_t*) frame)[i]);
    }
}

void print_board(uint32_t board, char *color) {
    switch (board) {
        case COM_GRAZYNA_ID:
            printf("\x01b[48;5;9mGrazyna");
            break;
        case COM_STASZEK_ID:
            printf("\x01b[48;5;6mStaszek");
            break;
        case COM_RADEK_ID:
            printf("\x01b[48;5;2mRadek");
            break;
        case COM_CZAPLA_ID:
            printf("\x01b[48;5;3mCzapla");
            break;
        case COM_PAUEK_ID:
            printf("\x01b[48;5;4mPauek");
            break;
        case COM_KROMEK_ID:
            printf("\x01b[48;5;5mKromek");
            break;
        default:
            printf("\x01b[41mid: %lu", board);
    }
    printf("%s", color);
}

void print_action(uint32_t action) {
    switch (action) {
        case COM_FEED:
            printf("feed");
            break;
        case COM_SERVICE:
            printf("serv");
            break;
        case COM_ACK:
            printf("ack");
            break;
        case COM_NACK:
            printf("nack");
            break;
        case COM_HEARTBEAT:
            printf("beat");
            break;
        case COM_REQUEST:
            printf("req");
            break;
        case COM_RESPONSE:
            printf("res");
            break;
        default:
            printf("unknown");
    }
}

void print_payload(ComFrame *frame) {
    if (frame->data_type == NO_DATA) return;
    printf("\t Data: ");
    switch (frame->data_type) {
        case NO_DATA:
            break;
        case UINT8:
        case UINT16:
            printf("%u", (uint16_t) frame->payload);
            break;
        case UINT32:
            printf("%lu", frame->payload);
            break;
        case INT8:
        case INT16:
            printf("%d", (uint16_t) frame->payload);
            break;
        case INT32:
            printf("%ld", frame->payload);
            break;
        case FLOAT:;
            printf("%f", *((float*) &frame->payload));
            break;
    }
}

void SS_com_debug_print_frame(ComFrame *frame, char *title, char *color) {
    ComFrame buf;
    memcpy(&buf, frame, sizeof(ComFrame));
    printf("%s\r\n%s\t", color, title);
    printf("From: ");
    print_board(frame->source, color);
    printf("\t To: ");
    print_board(frame->destination, color);
    printf("\t Priority: %u ", frame->priority);
    printf("\t action: ");
    print_action(frame->action);
    printf("\t device: 0x%02x", frame->device);
    printf("\t id: 0x%02x", frame->id);
    printf("\t type: 0x%02x%s", frame->message_type, color);
    print_payload(frame);
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

//#endif
