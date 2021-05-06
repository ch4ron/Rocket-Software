/*
 * SS_com_debug.c
 *
 *  Created on: Jan 18, 2020
 *      Author: maciek
 */

/* ==================================================================== */
/* ============================= Includes ============================= */
/* ==================================================================== */

#include "SS_com_debug.h"

#include "SS_com_ids.h"
#include "SS_log.h"
#include "portable.h"
#include "printf.h"
#include "string.h"
#ifdef SS_USE_GRAZYNA
#include "SS_grazyna.h"
#endif


/* ==================================================================== */
/* ========================= Private macros =========================== */
/* ==================================================================== */

#define COLOR_RESET "\x001b[0m"
#define BOLD "\x001b[1m"

#define COLOR_RED(str) "\x01b[38;5;1m" #str
#define COLOR_GREEN(str) "\x01b[38;5;2m" #str
#define COLOR_YELLOW(str) "\x01b[38;5;3m" #str
#define COLOR_BLUE(str) "\x01b[38;5;4m" #str
#define COLOR_VIOLET(str) "\x01b[38;5;5m" #str
#define COLOR_AZURE(str) "\x01b[38;5;6m" #str
#define COLOR_PINK(str) "\x01b[38;5;9m" #str
#define COLOR_ORANGE(str) "\x01b[38;5;208m" #str

#define COLOR_BG_RED "\x01b[48;5;1m"
#define COLOR_BG_GREEN "\x01b[48;5;2m"
#define COLOR_BG_YELLOW "\x01b[48;5;3m"
#define COLOR_BG_BLUE "\x01b[48;5;4m"
#define COLOR_BG_VIOLET "\x01b[48;5;5m"
#define COLOR_BG_AZURE "\x01b[48;5;6m"
#define COLOR_BG_PINK "\x01b[48;5;9m"
#define COLOR_BG_ORANGE "\x01b[48;5;208m"

/* ==================================================================== */
/* =================== Private function prototypes ==================== */
/* ==================================================================== */

#ifdef SS_DEBUG_ENABLED

static char *SS_com_board_str(ComBoardID board);
static char *SS_com_action_str(ComActionID action);
static char *SS_com_device_str(ComDeviceID device);
static char *SS_com_data_type_str(ComDataType type);
static void SS_com_debug_print_payload(ComFrame *frame);
static void SS_com_debug_print_frame(ComFrame *frame, char *title, char *color);

/* ==================================================================== */
/* ========================= Public functions ========================= */
/* ==================================================================== */

void _SS_com_print_message_received(ComFrame *frame) {
    char color[] = "\x01b[48;5;238m";
    char title[] = "Rec:";
    SS_com_debug_print_frame(frame, title, color);
}

void _SS_com_print_message_sent(ComFrame *frame) {
    char color[] = "\x01b[48;5;234m";
    char title[] = "Sent:";
    SS_com_debug_print_frame(frame, title, color);
}

void _SS_com_print_message_error(ComFrame *frame, char *error) {
    char color[] = "\x01b[41m";
    SS_com_debug_print_frame(frame, error, color);
}

/* ==================================================================== */
/* ======================== Private functions ========================= */
/* ==================================================================== */

static char *SS_com_board_str(ComBoardID board) {
    switch(board) {
        case COM_GRAZYNA_ID:
            return COLOR_AZURE(Grazyna);
        case COM_STASZEK_ID:
            return COLOR_GREEN(Staszek);
        case COM_RADEK_ID:
            return COLOR_YELLOW(Radek);
        case COM_CZAPLA_ID:
            return COLOR_BLUE(Czapla);
        case COM_PAUEK_ID:
            return COLOR_PINK(Pauek);
        case COM_KROMEK_ID:
            return COLOR_VIOLET(Kromek);
        case COM_BROADCAST_ID:
            return COLOR_ORANGE(Broad);
        default:
            return COLOR_RED(err);
    }
}

static char *SS_com_action_str(ComActionID action) {
    switch(action) {
        case COM_FEED:
            return "feed";
        case COM_SERVICE:
            return "serv";
        case COM_ACK:
            return "ack";
        case COM_NACK:
            return "nack";
        case COM_SACK:
            return "sack";
        case COM_SNACK:
            return "snack";
        case COM_HEARTBEAT:
            return "beat";
        case COM_REQUEST:
            return "req";
        case COM_RESPONSE:
            return "res";
        case COM_SEQUENCE:
            return "seq";
        default:
            SS_println("%d", action);
            return COLOR_RED(err);
    }
}

static char *SS_com_device_str(ComDeviceID device) {

    switch(device) {
        case COM_SERVO_ID:
            return "servo";
        case COM_RELAY_ID:
            return "relay";
        case COM_MEASUREMENT_ID:
            return "meas";
        case COM_SUPPLY_ID:
            return "supply";
        case COM_MEMORY_ID:
            return "memory";
        case COM_IGNITER_ID:
            return "igniter";
        case COM_FLASH_ID:
            return "flash";
        case COM_MPU9250_ID:
            return "mpu";
        case COM_DYNAMIXEL_ID:
            return "dynamixel";
        case COM_SEQUENCE_ID:
            return "seq";
        default:
            return COLOR_RED(err);
    }
}

static char *SS_com_data_type_str(ComDataType type) {
    switch(type) {
        case NO_DATA:
            return "empty";
        case UINT32:
            return "uint32";
        case UINT16:
            return "uint16";
        case UINT8:
            return "uint8";
        case INT32:
            return "int32";
        case INT16:
            return "int16";
        case INT8:
            return "int8";
        case FLOAT:
            return "float";
        case INT16x2:
            return "int16x2";
        default:
            return COLOR_RED(err);
    }
}

static void SS_com_debug_print_payload(ComFrame *frame) {
    float f_val;
    int16_t vals[2];
    if(frame->data_type == NO_DATA) return;
    SS_print_no_flush("type: %-9s", SS_com_data_type_str(frame->data_type));
    SS_print_no_flush("data: ");
    switch(frame->data_type) {
        case NO_DATA:
            break;
        case UINT8:
        case UINT16:
        case UINT32:
            SS_print_no_flush("%u", (uint16_t) frame->payload);
            break;
        case INT8:
        case INT16:
        case INT32:
            SS_print_no_flush("%d", (uint16_t) frame->payload);
            break;
        case FLOAT:
            memcpy(&f_val, &frame->payload, sizeof(float));
            SS_print_no_flush("%f", f_val);
            break;
        case INT16x2:
            memcpy(vals, &frame->payload, sizeof(vals));
            SS_print_no_flush("%d:%d", vals[0], vals[1]);
            break;
    }
}

static void SS_com_debug_print_frame(ComFrame *frame, char *title, char *color) {
    SS_print_no_flush_start();
    SS_print_no_flush("%s%-8", color, title);
    SS_print_no_flush("src: %-16s%s%s ", SS_com_board_str(frame->source), COLOR_RESET, color);
    SS_print_no_flush("dst: %-16s%s%s ", SS_com_board_str(frame->destination), COLOR_RESET, color);
    SS_print_no_flush("pri: %u ", frame->priority);
    SS_print_no_flush("act: %-6s", SS_com_action_str(frame->action));
    SS_print_no_flush("dev: %-10s", SS_com_device_str(frame->device));
    SS_print_no_flush("id: 0x%02x ", frame->id);
    SS_print_no_flush("op: 0x%02x ", frame->operation);
    SS_com_debug_print_payload(frame);
    SS_print_no_flush("\r\n");
    SS_log_buf_flush();
    SS_print_no_flush_end();
}

#ifdef SS_COM_DEBUG_HEX

/* ==================================================================== */
/* ============================== Misc ================================ */
/* ==================================================================== */

#ifdef SS_USE_GRAZYNA
void SS_grazyna_print_hex(GrazynaFrame *frame) {
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

void SS_com_print_hex(ComFrame *frame) {
    SS_print("\r\n");
    for(uint8_t i = 0; i < sizeof(ComFrame); i++) {
        SS_print("0x%02x ", ((uint8_t *) frame)[i]);
    }
}
#endif

#endif // SS_DEBUG_ENABLED
