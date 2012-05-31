#define USB_NO_ERROR 0
#define USB_ERROR 1

#define true 1
#define false 0

typedef char bool;

int init_usb (void);
int close_usb (void);
int read_debug_byte (int *value);


int set_led (int value);
