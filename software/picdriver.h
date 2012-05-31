#define USB_NO_ERROR 0
#define USB_ERROR 1

int init_usb (void);
int close_usb (void);
int read_analog_input (int *value);
int set_led (int value);
