#define SUCCESS 0
#define USB_ERROR 1
#define CONFIG_ERROR 2

#define true 1
#define false 0

#define DEBUG

typedef char bool;

int init_usb (void);
int close_usb (void);
int read_debug_byte (int *value);

int send_config_message(bool async, bool sync, bool rising, bool falling, bool both, unsigned long rate, unsigned long samplenumber);

//int set_led (int value);
