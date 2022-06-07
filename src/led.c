#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h> // memset

// paths to configure GPIO pins
#define FILE_GPIO_EXPORT    "/sys/class/gpio/export"
#define FILE_GPIO_DIRECTION "/sys/class/gpio/gpio%d/direction"
#define FILE_GPIO_VALUE     "/sys/class/gpio/gpio%d/value"

// GPIO pin toggles
#define PIN_LOW                 0
#define PIN_HIGH                1

// GPIO direction toggles
#define GPIO_DIRECTION_OUTPUT   0
#define GPIO_DIRECTION_INPUT    1

// GPIO pins
#define GPIO_CHIP           1100
#define M0054_DS2_LED_RED   82 + GPIO_CHIP
#define M0054_DS2_LED_GREEN 83 + GPIO_CHIP
#define M0054_DS2_LED_BLUE  84 + GPIO_CHIP

static int en_debug = 0;

// control gpio pins by specify which gpio to configure, its direction, and value to R/W based on direction
// when direction is set to output gpio_value is used to write, when set to input, gpio_value is set to read value
static int _gpio_control(unsigned int gpio_pin, int direction, unsigned int *gpio_value){
    int fd = -1;
    
    char read_buf[2] = {2};
    char write_buf[5] = {0};
    char gpio_file_name[128] = {0};

    // set write buf to pin number for export
    snprintf(write_buf, sizeof(write_buf), "%d", gpio_pin);

    // open export path
    fd = open(FILE_GPIO_EXPORT, O_WRONLY);
    if (fd < 0){
        printf("ERROR opening export file %s for pin %d\n", gpio_file_name, gpio_pin);
        return -1;
    }

    // export gpio pin
    write(fd, write_buf, sizeof(write_buf));
    close(fd);
    
    // debug prints
    if (en_debug) printf("Writing %s to %s for gpio pin %d\n", write_buf, FILE_GPIO_EXPORT, gpio_pin);

    // open
    snprintf(gpio_file_name, sizeof(gpio_file_name), FILE_GPIO_DIRECTION, gpio_pin);
    fd = open(gpio_file_name, O_RDWR);
    if (fd < 0){ 
        printf("ERROR opening direction file: %s\n", gpio_file_name);
        return -1;
    }

    memset(write_buf, 0, sizeof(write_buf)); // reset write buffer

    // set pin direction
    if (direction == GPIO_DIRECTION_INPUT){
        // set direction
        snprintf(write_buf, sizeof(write_buf), "%s", "in");
        write(fd, write_buf, sizeof(write_buf));
        close(fd);
        
        // debug prints
        if (en_debug) printf("Writing %s to %s for gpio pin %d\n", write_buf, FILE_GPIO_DIRECTION, gpio_pin);

        // open value file
        snprintf(gpio_file_name, sizeof(gpio_file_name), FILE_GPIO_VALUE, gpio_pin);
        fd = open(gpio_file_name, O_RDONLY);
        if (fd < 0){
            printf("ERROR failed to open value file: %s\n", gpio_file_name);
        }

        // read value from GPIO
        read(fd, read_buf, sizeof(read_buf));
        close(fd);

        sscanf(read_buf, "%d", gpio_value);

        // debug prints
        if (en_debug) printf("read %d from gpio pin %d\n", *gpio_value, gpio_pin);
    }
    else if(direction == GPIO_DIRECTION_OUTPUT){
        // set direction
        snprintf(write_buf, sizeof(write_buf), "%s", "out");
        write(fd, write_buf, sizeof(write_buf));
        close(fd);

        // open value file
        snprintf(gpio_file_name, sizeof(gpio_file_name), FILE_GPIO_VALUE, gpio_pin);
        fd = open(gpio_file_name, O_WRONLY);
        if (fd < 0){
            printf("ERROR failed to open value file: %s\n", gpio_file_name);
        }

        sprintf(write_buf, "%d", *gpio_value); // int to str
        write(fd, write_buf, sizeof(write_buf));
        close(fd);

        // debug prints
        if (en_debug) printf("Writing %s to %s for gpio pin %d\n", write_buf, FILE_GPIO_VALUE, gpio_pin);
    }
    else{
        printf("ERROR pin direction is not supported\n");
    }

    return 0;
}

void led_m0054_red(int en){
	unsigned int gpio_value = en ? PIN_HIGH : PIN_LOW;
	_gpio_control(M0054_DS2_LED_BLUE, GPIO_DIRECTION_OUTPUT, &gpio_value);
}

void led_m0054_green(int en){
	unsigned int gpio_value = en ? PIN_HIGH : PIN_LOW;
	_gpio_control(M0054_DS2_LED_BLUE, GPIO_DIRECTION_OUTPUT, &gpio_value);
}

void led_m0054_blue(int en){
	unsigned int gpio_value = en ? PIN_HIGH : PIN_LOW;
	_gpio_control(M0054_DS2_LED_BLUE, GPIO_DIRECTION_OUTPUT, &gpio_value);
}