/*
 * Copyright (c) 2024 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <termios.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/select.h>
#include <byteswap.h>
#include <poll.h>
#include "hw_rome.h"
#include "hci_uart.h"
#define PATH_MAX    50

extern vnd_userial_cb_t vnd_userial;

enum HciPacketType {
  HCI_PACKET_TYPE_COMMAND = 1,
  HCI_PACKET_TYPE_ACL_DATA = 2,
  HCI_PACKET_TYPE_SCO_DATA = 3,
  HCI_PACKET_TYPE_EVENT = 4
};

char patchFileLoc[PATH_MAX] = "";
char nvmFileLoc[PATH_MAX] = "";
char mbnFileLoc[PATH_MAX] = "";



static const char *rawcmd_help =
"Usage:\n"
"btconfig BT-PORT=/dev/ttyMSM1 BT-BAUDRATE=115200 Patch=Loc NVM=/bluetooth/msnv11.bin enable_scans enable_log\n";
/*
/tmp/ttyMSM1
cat /tmp/ttyMSM1

*/
static int uart_speed(int s)
{
    switch (s) {
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        case 500000:
            return B500000;
        case 576000:
            return B576000;
        case 921600:
            return B921600;
        case 1000000:
            return B1000000;
        case 1152000:
            return B1152000;
        case 1500000:
            return B1500000;
        case 2000000:
            return B2000000;
#ifdef B2500000
        case 2500000:
            return B2500000;
#endif
        case 3000000:
            return B3000000;
#ifdef B3500000
        case 3500000:
            return B3500000;
#endif
#ifdef B4000000
        case 4000000:
            return B4000000;
#endif
        default:
            return B57600;
    }
}


int set_speed(struct termios *ti, int speed)
{
    if (cfsetospeed(ti, uart_speed(speed)) < 0)
        return -errno;

    if (cfsetispeed(ti, uart_speed(speed)) < 0)
        return -errno;

    if (tcsetattr(vnd_userial.fd, TCSANOW, ti) < 0)
        return -errno;

    return 0;
}

int initUartBTPort(char* device,int baudToUse)
{
    struct termios ti;
    unsigned long flags = 0;

    printf("initUartBTPort: using device=%s, bandrate=%d to init uart port.\n", device, baudToUse);
    if((vnd_userial.fd = open(device, O_RDWR | O_NOCTTY)) == -1)
    {
        printf("%s: Open serial port %s failed. return=%d, errno=%d - (%s)\n", __func__, device, vnd_userial.fd, errno, strerror(errno));
        return -1;
    }
    tcflush(vnd_userial.fd, TCIOFLUSH);

    if (tcgetattr(vnd_userial.fd, &vnd_userial.termios) < 0) {
        printf("Can't get port settings\n");
        return -1;
    }

    cfmakeraw(&vnd_userial.termios);

    vnd_userial.termios.c_cflag |= CLOCAL;
    vnd_userial.termios.c_cflag |= CRTSCTS;

    vnd_userial.termios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK| ISTRIP | INLCR
	                 | IGNCR | ICRNL | IXON);

    vnd_userial.termios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

    vnd_userial.termios.c_cflag &= ~(CSIZE | PARENB | CBAUD);
    vnd_userial.termios.c_oflag &= ~(OPOST);
    vnd_userial.termios.c_cflag |= CS8;
    vnd_userial.termios.c_cc[VMIN] = 0;
    vnd_userial.termios.c_cc[VTIME] = 255;

    if (tcsetattr(vnd_userial.fd, TCSANOW, &vnd_userial.termios) < 0) {
        printf("Can't set port settings\n");
        close(vnd_userial.fd);
        return -1;
    }
    /* Set initial baudrate */
    if (set_speed(&vnd_userial.termios, baudToUse) < 0) {
        printf("Can't set initial baud rate\n");
        close(vnd_userial.fd);
        return -1;
    }

    tcflush(vnd_userial.fd, TCIOFLUSH);


    if (ioctl(vnd_userial.fd, TIOCMGET, &flags) < 0) {
        printf("TIOCMGET failed in init\n");
        close(vnd_userial.fd);
        return -1;
    }

    flags |= TIOCM_RTS;
    if (ioctl(vnd_userial.fd, TIOCMSET, &flags) < 0) {
        printf("\nTIOCMSET failed in init: HW Flow-on error\n");
        close(vnd_userial.fd);
        return -1;
    }

    if (ioctl(vnd_userial.fd, TIOCMGET, &flags) < 0) {
        printf("TIOCMGET failed in init\n");
        close(vnd_userial.fd);
        return -1;
    }
    if(!(flags & TIOCM_RTS))
    {
        printf("problem\n");
    }

    tcflush(vnd_userial.fd, TCIOFLUSH);
    printf("initUartBTPort: All termois settings are done successfully\n");
    return vnd_userial.fd;
}

extern int enable_scans;
extern boolean print_enabled;

int main(int argc , char *argv[])
{
    char deviceBtp[PATH_MAX] = "";
    char* pPosition = NULL;
    char baudrate[10] = "115200";    //default: 115200
    int baudToUse = 115200;

    printf("btconfig: BT Patch/NVM download application\n");
    if (argc < 5)
    {
        printf("\n%s\n",rawcmd_help);
        return 1;
    }

    //bluetooth serial port to York
    if (NULL == strstr(argv[1], "BT-PORT"))
    {
        printf("Specified bluetooth serial York port to attach is not valid\n");
        return 1;
    }
    else
    {
        pPosition = strchr(argv[1], '=');
        memcpy(deviceBtp, pPosition+1, strlen(pPosition+1));
    }

    //bluetooth serial qcrpg port baudrate.
    if (NULL == strstr(argv[2], "BT-BAUDRATE"))
    {
        printf("Missing baudrate option\n");
        return 1;
    }
    else
    {
        pPosition = strchr(argv[2], '=');
        memcpy(baudrate, pPosition+1, strlen(pPosition+1));
        baudToUse = atoi(baudrate);
    }

    /* Bluetooth patch file location */
    if (NULL == strstr(argv[3], "PATCH"))
    {
        printf("Specified Patch file location is not valid\n");
        return 1;
    }
    else
    {
        pPosition = strchr(argv[3], '=');
        memcpy(patchFileLoc, pPosition+1, strlen(pPosition+1));
    }

    /* Bluetooth NVM file location */
    if (NULL == strstr(argv[4], "NVM"))
    {
        printf("Specified NVM file location is not valid\n");
        return 1;
    }
    else
    {
        pPosition = strchr(argv[4], '=');
        memcpy(nvmFileLoc, pPosition+1, strlen(pPosition+1));
    }
    enable_scans = atoi(argv[5]);
    if(enable_scans)
    {
        printf("\nEnable Scans set\n");
    }
    else
    {
        printf("\nEnable Scans Not set\n");
    }

    print_enabled = atoi(argv[6]);
    if(print_enabled)
    {
        printf("\nprint_enabled set\n");
    }
    else
    {
        printf("\nprint_enabled Not set\n");
    }

    rampatch_file_path = patchFileLoc;
    nvm_file_path = nvmFileLoc;
    mbn_file_path = mbnFileLoc;
    if(initUartBTPort(deviceBtp,baudToUse) < 0)
        printf("initUartBTPort failed\n");


    printf("\n%s - rome_soc_init called\n", __FUNCTION__);

    rome_soc_init(vnd_userial.fd, NULL);

    close(vnd_userial.fd);
    return 0;
}

