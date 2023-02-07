#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include "segDisplay.h"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20
// For Zen Cape Green
#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15
// For Zen Cape Red
#define RED_REG_DIRA 0x02
#define RED_REG_DIRB 0x03
#define RED_REG_OUTA 0x00
#define RED_REG_OUTB 0x01

#define I2C_RIGHT_DIGIT "/sys/class/gpio/gpio44/value"
#define I2C_LEFT_DIGIT "/sys/class/gpio/gpio61/value"

#define CONFIG_P9_18 "config-pin P9_18 i2c"
#define CONFIG_P9_17 "config-pin P9_17 i2c"
#define SET_DIRECTION_GPIO_61 "echo out > /sys/class/gpio/gpio61/direction"
#define SET_DIRECTION_GPIO_44 "echo out > /sys/class/gpio/gpio44/direction"
#define TURN_ON_GPIO_61 "echo 1 > /sys/class/gpio/gpio61/value"
#define TURN_ON_GPIO_44 "echo 1 > /sys/class/gpio/gpio44/value"

#define SECONDS 0
#define NANOSECONDS 5000000

pthread_t segDisplayThread;
struct timespec reqDelay = {
    SECONDS,
    NANOSECONDS};

// Green Zen Cape
// static int digitReg[10][2] = {{0x86, 0xA1}, {0x12, 0x80}, {0x0F, 0x31}, {0x06, 0xB0}, {0x8A, 0x90}, {0x8C, 0xB0}, {0x8C, 0xB1}, {0x14, 0x04}, {0x8E, 0xB1}, {0x8E, 0x90}};
// Red Zen Cape
static int digitReg[10][2] = {{0xD0, 0xA1}, {0x00, 0xA0}, {0x98, 0x83}, {0x18, 0xA3}, {0x48, 0xA2}, {0x58, 0x63}, {0xD8, 0x63}, {0x50, 0xA0}, {0xD8, 0xA3}, {0x58, 0xA3}};

// Used to run commands on a terminal (for config I2C and GPIO setup)
static void runCommand(char *command)
{
    // Execute command
    FILE *pipe = popen(command, "r");

    // Ignore output, consume it - no error when closing pipe
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe))
    {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
        {
            break;
        }
    }

    // Get exit code from pipe; non-zero is an error
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0)
    {
        perror("Unable to execute command:");
        printf("    command:    %s\n", command);
        printf("    exit code: %d\n", exitCode);
    }
}

// Open File to Write 0 and 1 for Left & Right Segment Display
static void openFileToWrite(char *filename, char *writeStr)
{
    FILE *pFile = fopen(filename, "w");
    if (pFile == NULL)
    {
        printf("ERROR OPENING %s.", filename);
        exit(1);
    }

    int charWritten = fprintf(pFile, writeStr);
    if (charWritten <= 0)
    {
        printf("ERROR WRITING DATA TO %s.", filename);
        exit(1);
    }
    fclose(pFile);
}

// Initialize I2C Bus
static int initI2cBus(char *bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    if (i2cFileDesc < 0)
    {
        printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
        perror("Error is:");
        exit(-1);
    }

    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0)
    {
        perror("Unable to set I2C device to slave address.");
        exit(-1);
    }
    return i2cFileDesc;
}

// Write I2C Register
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2)
    {
        perror("Unable to write i2c register");
        exit(-1);
    }
}

// Turn off the Segement Display
void Seg_turnOffSegDisplay(void)
{
    openFileToWrite(I2C_RIGHT_DIGIT, "0");
    openFileToWrite(I2C_LEFT_DIGIT, "0");
}

// Turon on and Display the Number of Dips on the 14 segment
void Seg_displayDigits(int displayDigit, char *i2cDigitDirectionPath)
{
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    // Green Zen Cape
    // writeI2cReg(i2cFileDesc, RED_REG_DIRA, 0x00);
    // writeI2cReg(i2cFileDesc, RED_REG_DIRB, 0x00);

    // Red Zen Cape
    writeI2cReg(i2cFileDesc, RED_REG_DIRA, 0x00);
    writeI2cReg(i2cFileDesc, RED_REG_DIRB, 0x00);

    // Green Zen Cape
    // writeI2cReg(i2cFileDesc, REG_OUTA, digitReg[displayDigit][0]);
    // writeI2cReg(i2cFileDesc, REG_OUTB, digitReg[displayDigit][1]);

    // Red Zen Cape
    writeI2cReg(i2cFileDesc, RED_REG_OUTA, digitReg[displayDigit][0]);
    writeI2cReg(i2cFileDesc, RED_REG_OUTB, digitReg[displayDigit][1]);

    openFileToWrite(i2cDigitDirectionPath, "1");
    nanosleep(&reqDelay, (struct timespec *)NULL);
    close(i2cFileDesc);
    return;
}

// Initialize the Segment Setting (Config I2C and Export GPIO)
void Seg_init(void)
{
    char *configArray[] = {CONFIG_P9_18, CONFIG_P9_17,
                           SET_DIRECTION_GPIO_61, SET_DIRECTION_GPIO_44,
                           TURN_ON_GPIO_61, TURN_ON_GPIO_44};
    for (int i = 0; i < sizeof(configArray) / sizeof(configArray[0]); i++)
    {
        runCommand(configArray[i]);
    }
    pthread_join(segDisplayThread, NULL);
    return;
}

void * segDisplay_threadFunc(){
    Seg_init();
    return NULL;
}

void segDisplay_threadInit() {
    pthread_create(segDisplayThread, NULL, segDisplay_threadFunc, NULL);
}