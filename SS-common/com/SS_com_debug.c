/*
 * SS_com_debug.c
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

#include "SS_com_debug.h"
#include "string.h"
#include "stdio.h"

//#ifdef CAN_DEBUG

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

void print_payload(ComFrameContent *frame) {
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

void SS_com_debug_print_frame(ComFrameContent *frame, char *title, char *color) {
    ComFrameContent buf;
    memcpy(&buf, frame, sizeof(ComFrameContent));
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
    printf("\t type: 0x%02x", frame->message_type);
    char *grazyna_ind = frame->grazyna_ind ? "\x01b[48;5;34my" : "\x01b[48;5;166mn";
    printf("\t grazyna ind: %s%s", grazyna_ind, color);
    print_payload(frame);
}

void SS_com_print_message_received(ComFrameContent *frame) {
    char color[] = "\x01b[48;5;8m";
    char title[] = "Rec:";
    SS_com_debug_print_frame(frame, title, color);
}

void SS_com_print_message_sent(ComFrameContent *frame) {
    char color[] = "\x01b[48;5;234m";
    char title[] = "Sent:";
    SS_com_debug_print_frame(frame, title, color);
}

void SS_com_print_message_error(ComFrameContent *frame, char *error) {
    char color[] = "\x01b[41m";
    SS_com_debug_print_frame(frame, error, color);
}

//#endif
