#ifndef COMMS_H
#define COMMS_H

#define true 1
#define false 0

#ifdef LOGICANALYSER_C
int debug;
#else
extern int debug;
#endif

typedef char bool;

int init_usb (void);
int close_usb (void);
int read_debug_byte (int *value);

int do_ping(void);

int send_config_message(bool async, bool sync, bool rising, bool falling, bool both,
	unsigned long rate, unsigned long samplenumber);
int send_arm_request(void);
int poll_state(unsigned int *sampleptr, unsigned int *state);
int getdata(char *datastore, char **datastoreptr);



#define DATASTORE_SIZE 131072

#define MARK 1
#define SPACE 0



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

#define CMD_ARM_RQ		0x60
#define LEN_ARM_RQ		0x02
#define CMD_ARM_RS		0x60
#define LEN_ARM_RS		0x03

#define CMD_POLL_RQ		0x65
#define LEN_POLL_RQ		0x02
#define CMD_POLL_RS		0x65
#define LEN_POLL_RS		0x07 // Variable len - 0x03 unless state=INPROGRESS

#define CMD_GETDATA_RQ	0x66
#define LEN_GETDATA_RQ	0x02
#define CMD_GETDATA_RS	0x66
#define LEN_GETDATA_RS	0x40 // Variable length - anywhere between 0x02 and 0x40 depending on
								// data len

// States returned in POLL_RS:

#define STATE_START		0x61 // We shouldn't really see this state but it might occur in some race conditions
								// so we should handle it gracefully.  Basically just ignore it.
#define STATE_WAIT		0x62 // Either waiting for async trigger or waiting for first sync clock. No samps yet captured
#define STATE_PROG		0x63 // Capture is in progress - we have captured at least one sample.
								// This state returns a longer packet with the number of samples captured as payload
#define STATE_FIN		0x64 // Capture has finished - PIC is hopefully ready for retrieve.



// Response codes used internally:

#define SUCCESS			0x00
#define USB_ERROR		0x01
#define CONFIG_ERROR	0x02
#define ARM_ERROR		0x03
#define GETDATA_SUCCESS	0x04
#define GETDATA_EOF		0x05
#define GETDATA_ERROR	0x06

// Response codes from the PIC:

#define CONFIG_SUCCESS	0x01
#define CONFIG_FAIL		0x00

#define ARM_SUCCESS		0x01
#define ARM_FAIL		0x00

// Error command payloads from PIC (copied directly from bottom of firmware/user/user.h
#define	ERROR_PAYLOAD_CMD_NOT_FOUND				0x01
#define ERROR_PAYLOAD_INVALID_SAMPLE_RATE		0x02
#define ERROR_PAYLOAD_INVALID_SAMPLE_NUMBER		0x03
#define ERROR_PAYLOAD_INVALID_CONFIG			0x04
#define ERROR_PAYLOAD_DATA_UNAVAILABLE			0x05



#endif
