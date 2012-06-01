#ifndef PICDRIVER_H
#define PICDRIVER_H

#define SUCCESS 0
#define USB_ERROR 1
#define CONFIG_ERROR 2

#define true 1
#define false 0

static int debug=1;

typedef char bool;

int init_usb (void);
int close_usb (void);
int read_debug_byte (int *value);

int send_config_message(bool async, bool sync, bool rising, bool falling, bool both, unsigned long rate, unsigned long samplenumber);

//int set_led (int value);



// Various commands - described in spec.ods in the repo root.
// RQ = request PC->PIC
// RS = response PIC->PC
// LEN = length of command inc command and length bytes (i.e. minimum is 2 w/ no payload)
#define CMD_CONFIG_RQ	0x42
#define LEN_CONFIG_RQ	0x0B
#define CMD_CONFIG_RS	0x42
#define LEN_CONFIG_RS	0x03

#define CMD_ERROR_RS	0xAA
#define LEN_ERROR_RS	0x03

#define CMD_DEBUG_RQ	0xED
#define LEN_DEBUG_RQ	0x02
#define CMD_DEBUG_RS	0xED
#define LEN_DEBUG_RS	0x03

#define CMD_PING_RQ		0xDD
#define LEN_PING_RQ		0x02
#define CMD_PING_RS		0xDD
#define LEN_PING_RS		0x02

#define CONFIG_SUCCESS	0x01
#define CONFIG_FAIL		0x00



#endif
