#ifndef AX25_H_
#define AX25_H_

// ---------------->> INCLUDES:
#include "main.h"
#include "config.h"
#include "afsk.h"
// ----------------<< INCLUDES


// ---------------->> DEFINES:
// ----------------<< DEFINES


// ---------------->> OTHER:
// ----------------<< OTHER


// ---------------->> VARIABLES:
struct s_address {
	char callsign[7];
	uint8_t ssid;
};
// ----------------<< VARIABLES


// ---------------->> FUNC:

void AX25_send_byte(uint8_t a_byte);
void AX25_send_flag(void);
void AX25_send_string(const char *string);
void AX25_send_header(const struct s_address *addresses, uint16_t num_addresses);
void AX25_send_footer(void);
void AX25_flush_frame(void);

// ----------------<< FUNC


#endif /* AX25_H_ */
