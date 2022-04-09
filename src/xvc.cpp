#include <unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include "jtag.hpp"
#include "part.hpp"
#include "xvc.hpp"



using namespace std;

XVC::XVC(Jtag *jtag, string interface, int port, int verbose, int frequency):
    _jtag(jtag), _verbose(verbose), _frequency(frequency)
{
    uint32_t idcode = _jtag->get_target_device_id();
    string family = fpga_list[idcode].family;

    if (family.substr(0, 5) == "artix") {
        _fpga_family = ARTIX_FAMILY;
    } else if (family == "spartan7") {
        _fpga_family = SPARTAN7_FAMILY;
    } else if (family == "zynq") {
        _fpga_family = ZYNQ_FAMILY;
    } else if (family.substr(0, 6) == "zynqmp") {
        if (!zynqmp_init(family))
            throw std::runtime_error("Error with ZynqMP init");
        _fpga_family = ZYNQMP_FAMILY;
    } else if (family == "kintex7") {
        _fpga_family = KINTEX_FAMILY;
    } else if (family == "kintexus") {
        _fpga_family = KINTEXUS_FAMILY;
    } else if (family.substr(0, 8) == "spartan3") {
        _fpga_family = SPARTAN3_FAMILY;
    } else if (family == "xcf") {
        _fpga_family = XCF_FAMILY;
    } else if (family == "spartan6") {
        _fpga_family = SPARTAN6_FAMILY;
    } else if (family == "xc2c") {
        //xc2c_init(idcode);
    } else if (family == "xc9500xl") {
        _fpga_family = XC95_FAMILY;
    } else {
        _fpga_family = UNKNOWN_FAMILY;
    }

    cout << "Connected Product family is: "<< family << endl;
    //create socket
    if ((_socket = createSocket(interface, port)) < 0) {
        printError("Error: Create Socket Fail");
    }
    socketloop();
}

bool XVC::zynqmp_init(const std::string &family)
{
    /* by default, at powering a zynqmp has
     * PL TAP and ARM DAP disabled
     * at this time only PS TAB and a dummy are seen
     * So first step is to enable PL and ARM
     */
    if (family == "zynqmp_cfgn") {
        /* PS TAP is the first device with 0xfffffe idcode */
        _jtag->device_select(0);
        /* send 0x03 into JTAG_CTRL register */
        uint16_t ircode = 0x824;
        _jtag->shiftIR(ircode & 0xff, 8, Jtag::SHIFT_IR);
        _jtag->shiftIR((ircode >> 8) & 0x0f, 4);
        uint8_t instr[4] = {0x3, 0, 0, 0};
        _jtag->shiftDR(instr, NULL, 32);
        /* synchronize everything by moving to TLR */
        _jtag->set_state(Jtag::TEST_LOGIC_RESET);
        _jtag->toggleClk(10);
        _jtag->set_state(Jtag::RUN_TEST_IDLE);
        _jtag->toggleClk(100);
        /* force again JTAG chain detection */
        _jtag->detectChain(5);
    }
    /* check if the chain is correctly configured:
     * 2 devices
     * PL at position 0
     * ARM at position 1
     */
    char mess[256];
    std::vector<int> listDev = _jtag->get_devices_list();
    if (listDev.size() != 2) {
        snprintf(mess, sizeof(mess), "ZynqMP error: wrong"
                                     " JTAG length: %zu instead of 2\n",
                 listDev.size());
        printError(mess);
        return false;
    }

    if (fpga_list[listDev[0]].family != "zynqmp") {
        snprintf(mess, sizeof(mess), "ZynqMP error: first device"
                                     " is not the PL TAP -> 0x%08x\n",
                 listDev[0]);
        printError(mess);
        return false;
    }

    if (listDev[1] != 0x5ba00477) {
        snprintf(mess, sizeof(mess), "ZynqMP error: second device"
                                     " is not the ARM DAP cortex A53 -> 0x%08x\n",
                 listDev[1]);
        printError(mess);
        return false;
    }

    _jtag->insert_first(0xdeadbeef, 6);
    _jtag->device_select(1);
    _irlen = 6;

    return true;
}
#define USER1	0x02
#define CFG_IN   0x05
#define USERCODE   0x08
#define IDCODE     0x09
#define ISC_ENABLE 0x10
#define JPROGRAM 0x0B
#define JSTART   0x0C
#define JSHUTDOWN 0x0D
#define ISC_PROGRAM 0x11
#define ISC_DISABLE 0x16
#define BYPASS   0xff


void XVC::reset()
{
    _jtag->shiftIR(JSHUTDOWN, 6);
    _jtag->shiftIR(JPROGRAM, 6);
    _jtag->set_state(Jtag::RUN_TEST_IDLE);
    _jtag->toggleClk(10000*12);

    _jtag->set_state(Jtag::RUN_TEST_IDLE);
    _jtag->toggleClk(2000);

    _jtag->shiftIR(BYPASS, 6);
    _jtag->set_state(Jtag::RUN_TEST_IDLE);
    _jtag->toggleClk(2000);
}


XVC::~XVC() {}

int XVC::clockSpeed(const char *str)
{
    double frequency;
    char *endp;
    frequency = strtod(str, &endp);
    if ((endp == str)
        || ((*endp != '\0') && (*endp != 'M') && (*endp != 'k'))
        || ((*endp != '\0') && (*(endp+1) != '\0'))) {
        fprintf(stderr, "Bad clock frequency argument.\n");
        exit(2);
    }
    if (*endp == 'M') frequency *= 1000000;
    if (*endp == 'k') frequency *= 1000;
    if (frequency >= INT_MAX) frequency = INT_MAX;
    if (frequency <= 0) frequency = 1;
    divisorForFrequency(frequency);
    return frequency;
}

int XVC::divisorForFrequency(unsigned int frequency)
{
    unsigned int divisor;
    unsigned int actual;
    double r;
    static unsigned int warned = ~0;

    if (frequency <= 0) frequency = 1;
    divisor = ((FTDI_CLOCK_RATE / 2) + (frequency - 1)) / frequency;
    if (divisor >= 0x10000) {
        divisor = 0x10000;
    }
    if (divisor < 1)  {
        divisor = 1;
    }
    actual = FTDI_CLOCK_RATE / (2 * divisor);
    r = (double)frequency / actual;
    if (warned != actual) {
        warned = actual;
        if ((r < 0.999) || (r > 1.001)) {
            fprintf(stderr, "Warning -- %d Hz clock requested, %d Hz actual\n",
                    frequency, actual);
        }
        if (actual < 500000) {
            fprintf(stderr, "Warning -- %d Hz clock is a slow choice.\n",
                    actual);
        }
    }
    return divisor;
}

int XVC::createSocket(string interface, int port)
{
    int s, o;
    struct sockaddr_in myAddr;
    //copy from sting into string array
    int n = interface.length();
    char interface_array[n + 1];
    strcpy(interface_array, interface.c_str());

    s = socket (AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        return -1;
    }
    o = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &o, sizeof o) < 0) {
        return -1;
    }
    memset (&myAddr, '\0', sizeof myAddr);
    myAddr.sin_family = AF_INET;

    if(interface[0]=='-')
    {
        myAddr.sin_addr.s_addr = INADDR_ANY;
    }else{
        cout << "The following internet interface will be opened: "<< interface << endl;
        if (inet_pton(AF_INET, interface_array, &myAddr.sin_addr) != 1) {
            printError("Error : Bad address " + interface + ", could not build socket");
            return -1;
        }
    }
    myAddr.sin_port = htons(port);
    //note,there is two bind, "str" and "socket" also has one, here we add :: to make that is socket
    if (::bind (s, (struct sockaddr *)&myAddr, sizeof myAddr) < 0) {
        printError("Error: Bind() failed: " + std::to_string(errno));
        return -1;
    }
    if (listen (s, 1) < 0) {
        printError("Error: Listen() failed: " + std::to_string(errno));
        return -1;
    }

    if(interface[0]=='-')
    {
        printSuccess("Socket has been successfully Created! with all interface and Port number: " + std::to_string(port));
    }else{
        printSuccess("Socket has been successfully Created! with interface IP: "+ interface + " and Port: "+ std::to_string(port));
    }
    return s;
}

void XVC::socketloop()
{
    for (;;){
        struct sockaddr_in farAddr;
        socklen_t addrlen = sizeof farAddr;
        FILE *fp;
        static xvcInfo usbWorkspace = {
                .vendorId = 0x0403,
                .productId = -1,
                .ftdiJTAGindex = 1
        };
        xvcInfo *usb =  &usbWorkspace;
        int fd = accept(_socket, (struct sockaddr *)&farAddr, &addrlen);
        if (fd < 0) {
            printError("Error: Can't accept connection: " + std::to_string(errno));
            exit(1);
        }

        fp = fdopen(fd, "r");
        if (fp == NULL) {
            printError("Error: fdopen failed:  " + std::to_string(errno));
            close(fd);
            exit(2);
        }
        else {
            processCommands(fp, fd, usb);
            fclose(fp);  /* Closes underlying socket, too */
        }

    }
}
/************************************* MISC ***************************/
void XVC::showBuf(const char *name, const unsigned char *buf, int numBytes)
{
    int i;
    printf("%s%4d:", name, numBytes);
    if (numBytes > 40) numBytes = 40;
    for (i = 0 ; i < numBytes ; i++) printf(" %02X", buf[i]);
    printf("\n");
}

void XVC::badEOF(void)
{
    printError("Error: Unexpected EOF!");
}

void XVC::badChar(void)
{
    printError("Error: Unexpected character!");
}

/*
 * Fetch 32 bit value from client
 */
int XVC::fetch32(FILE *fp, uint32_t *value)
{
    int i;
    uint32_t v;

    for (i = 0, v = 0 ; i < 32 ; i += 8) {
        int c = fgetc(fp);
        if (c == EOF) {
            badEOF();
            return 0;
        }
        v |= c << i;
    }
    *value = v;
    return 1;
}
/************************************* XVC ***************************/

int XVC::GetidCode()
{
    int id = 0;
    unsigned char tx_data[4]= {0x00, 0x00, 0x00, 0x00};
    unsigned char rx_data[4];
    _jtag->go_test_logic_reset();
   // _jtag->shiftIR(IDCODE, 6);
    _jtag->shiftDR(tx_data, rx_data, 32);
    showBuf("Reading ID is: ", rx_data, 4);
    _jtag->go_test_logic_reset();
    id = ((rx_data[0] & 0x000000ff) |
          ((rx_data[1] << 8) & 0x0000ff00) |
          ((rx_data[2] << 16) & 0x00ff0000) |
          ((rx_data[3] << 24) & 0xff000000));
    return id;
}

/*
 * Shift a client packet set of bits
 */
int XVC::shift(xvcInfo *usb, FILE *fp)
{
    uint32_t nBits, nBytes;
    //read the shift
    if (!fetch32(fp, &nBits)) {
        return 0;
    }
    if (nBits > USB_BUFSIZE) {
        usb->largestShiftRequest = nBits;
    }
    usb->bitCount += nBits;
    usb->shiftCount++;
    nBytes = (nBits + 7) / 8;
//    unsigned char tms;
    if (_verbose>0) {
        printf("shift:%d (Bits) or %d (Bytes)\n", (int)nBits, nBytes);
    }
    if (nBytes > XVC_BUFSIZE) {
        fprintf(stderr, "Client requested %u, max is %u\n", nBytes, XVC_BUFSIZE);
        exit(1);
    }
    if ((fread(usb->tmsBuf, 1, nBytes, fp) != nBytes)
        || (fread(usb->tdiBuf, 1, nBytes, fp) != nBytes)) {
        return 0;
    }
    if (_verbose>0) {
        showBuf("TMS", usb->tmsBuf, nBytes);
        showBuf("TDI", usb->tdiBuf, nBytes);
    }
    int nSendBytes = (_jtag->direct_read_write(usb->tmsBuf, usb->tdiBuf, usb->tdoBuf, nBits)+ 7) / 8;

//    int project_calculation = _jtag->calculateTMS(usb->tmsBuf, nBits);
//    printf("project stat is %d and current stat is %d\n", project_calculation, _jtag->_state);
//    if(_jtag->_state != _jtag->SHIFT_IR && _jtag->_state != _jtag->SHIFT_DR){
//        for (uint32_t i = 0; i < nBits; ++i)
//        {
//            int tms = !!(usb->tmsBuf[i/8] & (1<<(i&7)));
//            _jtag->setTMS(tms);
//        }
//        _jtag->flushTMS(false);
//    }else{
//        if(project_calculation!= _jtag->_state)
//        {
//            for (uint32_t i = 0; i < nBits; ++i)
//            {
//                int tms = !!(usb->tmsBuf[i/8] & (1<<(i&7)));
//                _jtag->setTMS(tms);
//            }
//            _jtag->flushTMS(false);
//        }else{
//            _jtag->read_write(usb->tdiBuf, usb->tdoBuf, nBits, 0);
//        }
//    }
//    if (_verbose>0) {
//        showBuf("TDO", usb->tdoBuf, nBytes);
//    }
//    if (_verbose>0) {
//        if (std::memcmp(usb->tdiBuf, usb->tdoBuf, nBytes)) {
//            printf("Loopback failed.\n");
//        }
//    }

    return nSendBytes;
}

/*
 * Fetch a known string
 */
int XVC::matchInput(FILE *fp, const char *str)
{
    while (*str) {
        int c = fgetc(fp);
        if (c == EOF) {
            badEOF();
            return 0;
        }
        if (c != *str) {
            fprintf(stderr, "Expected 0x%2x, got 0x%2x\n", *str, c);
            return 0;
        }
        str++;
    }
    return 1;
}

int XVC::reply(int fd, const unsigned char *buf, int len)
{
    if (write(fd, buf, len) != len) {
        printError("Error: reply failed: " + std::to_string(errno));
        return 0;
    }
    return 1;
}

/*
 * Return a 32 bit value
 */
int XVC::reply32(int fd, uint32_t value)
{
    int i;
    unsigned char cbuf[4];

    for (i = 0 ; i < 4 ; i++) {
        cbuf[i] = value;
        value >>= 8;
    }
    return reply(fd, cbuf, 4);
}

void XVC::processCommands(FILE *fp, int fd, xvcInfo *usb)
{
    int c;
//    GetidCode();
    for (;;) {
        switch(c = fgetc(fp)) {
            case 's':
                switch(c = fgetc(fp)) {
                    case 'e':
                    {
                        uint32_t num;
                        int frequency;
                        if (!matchInput(fp, "ttck:")) return;
                        if (!fetch32(fp, &num)) return;
                        frequency = 1000000000 / num;
                        if (_verbose>0) {
//                            printf("settck:%d  (%d Hz)\n", (int)num, frequency);
                            printf("Utilizing System Frequency Setting %d instead of settck:%d  (%d Hz)\n", _frequency, (int)num, frequency);
                        }
                        //TODO add jtag setup speed here
                        //if (!ftdiSetClockSpeed(usb, frequency)) return;
                        if (!reply32(fd, num)) return;
                    }
                        break;
                    case 'h':
                    {
                        int nBytes;
                        if (!matchInput(fp, "ift:")) return;
                        nBytes = shift(usb, fp);
                        if ((nBytes <= 0) || !reply(fd, usb->tdoBuf, nBytes)) {
                            return;
                        }
                    }
                        break;

                    default:
                        if (_verbose>0) {
                            printf("Bad second char 0x%02x\n", c);
                        }
                        badChar();
                        return;
                }
                break;

            case 'g':
                if (matchInput(fp, "etinfo:")) {
                    char cBuf[40];
                    int len;;
                    if (_verbose>0) {
                        printf("getinfo:\n");
                    }
                    len = sprintf(cBuf, "xvcServer_v1.0:%u\n", XVC_BUFSIZE);
                    if (reply(fd, (unsigned char *)cBuf, len)) {
                        break;
                    }
                }
                return;

            case EOF:
                return;

            default:
                if (_verbose>0) {
                    printf("Bad initial char 0x%02x\n", c);
                }
                badChar();
                return;
        }
    }
}


