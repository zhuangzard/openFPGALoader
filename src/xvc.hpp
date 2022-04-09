// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2019 Gwenhael Goavec-Merou <gwenhael.goavec-merou@trabucayre.com>
 */

#ifndef XVC_HPP
#define XVC_HPP

#include <string>
#include "jtag.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include "display.hpp"
using namespace std;

#define XVC_BUFSIZE         2048
#define USB_BUFSIZE         512
#define FTDI_CLOCK_RATE     60000000
#define IDSTRING_CAPACITY   100

typedef struct xvcInfo {
    /*
     * Diagnostics
     */
    int                    quietFlag;
    int                    runtFlag;
    int                    loopback;
    int                    showUSB;
    int                    showXVC;
    unsigned int           lockedSpeed;

    /*
     * Statistics
     */
    int                    statisticsFlag;
    int                    largestShiftRequest;
    int                    largestWriteRequest;
    int                    largestWriteSent;
    int                    largestReadRequest;
    uint64_t               shiftCount;
    uint64_t               chunkCount;
    uint64_t               bitCount;

    /*
     * Used to find matching device
     */
    int                    vendorId;
    int                    productId;
    const char            *serialNumber;

    /*
     * Matched device
     */
    int                    deviceVendorId;
    int                    deviceProductId;
    char                   deviceVendorString[IDSTRING_CAPACITY];
    char                   deviceProductString[IDSTRING_CAPACITY];
    char                   deviceSerialString[IDSTRING_CAPACITY];

    /*
     * Libusb hooks
     */
    libusb_context        *usb;
    libusb_device_handle  *handle;
    int                    bInterfaceNumber;
    int                    bInterfaceProtocol;
    int                    isConnected;
    int                    termChar;
    unsigned char          bTag;
    int                    bulkOutEndpointAddress;
    int                    bulkOutRequestSize;
    int                    bulkInEndpointAddress;
    int                    bulkInRequestSize;

    /*
     * FTDI info
     */
    int                    ftdiJTAGindex;
    const char            *gpioArgument;

    /*
     * I/O buffers
     */
    unsigned char          tmsBuf[XVC_BUFSIZE];
    unsigned char          tdiBuf[XVC_BUFSIZE];
    unsigned char          tdoBuf[XVC_BUFSIZE];
    int                    txCount;
    unsigned char          ioBuf[USB_BUFSIZE];
    unsigned char          rxBuf[USB_BUFSIZE];
    unsigned char          cmdBuf[USB_BUFSIZE];
} xvcInfo;

class XVC {
	public:
        XVC(Jtag *jtag, string interface, int port, int verbose, int frequency);
		~XVC();

		int createSocket(string interface, int port);
        void socketloop();
        void processCommands(FILE *fp, int fd, xvcInfo *usb);
        void showBuf(const char *name, const unsigned char *buf, int numBytes);
        void badEOF(void);
        void badChar(void);
        int fetch32(FILE *fp, uint32_t *value);
        int shift(xvcInfo *usb, FILE *fp);
        int matchInput(FILE *fp, const char *str);
        int reply(int fd, const unsigned char *buf, int len);
        int reply32(int fd, uint32_t value);
        int clockSpeed(const char *str);
        int divisorForFrequency(unsigned int frequency);
        Jtag *_jtag;
        int _verbose;
        int _socket;
        int _frequency;
        void reset();
        int GetidCode();
	protected:


	private:
    /* list of xilinx family devices */
        enum xilinx_family_t {
            XC95_FAMILY     = 0,
            XC2C_FAMILY,
            SPARTAN3_FAMILY,
            SPARTAN6_FAMILY,
            SPARTAN7_FAMILY,
            ARTIX_FAMILY,
            KINTEX_FAMILY,
            KINTEXUS_FAMILY,
            ZYNQ_FAMILY,
            ZYNQMP_FAMILY,
            XCF_FAMILY,
            UNKNOWN_FAMILY  = 999
        };

        xilinx_family_t _fpga_family; /**< used to store current family */
        bool zynqmp_init(const std::string &family);
        int _irlen; /**< IR bit length */
};

#endif
