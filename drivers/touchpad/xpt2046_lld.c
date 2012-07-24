#include "xpt2046_lld.h"

#ifdef TOUCHPAD_USE_XPT2046

__inline uint16_t lld_tpReadX(void) {
    uint8_t txbuf[1];
    uint8_t rxbuf[2];
    uint16_t x;

    txbuf[0] = 0xd0;
    TP_CS_LOW;
	spiSend(&SPID1, 1, txbuf);
    spiReceive(&SPID1, 2, rxbuf);
    TP_CS_HIGH;

    x = rxbuf[0] << 4;
    x |= rxbuf[1] >> 4;
    
    return x;
}

__inline uint16_t lld_tpReadY(void) {
    uint8_t txbuf[1];
    uint8_t rxbuf[2];
    uint16_t y;

    txbuf[0] = 0x90;
    TP_CS_LOW;
    spiSend(&SPID1, 1, txbuf);
    spiReceive(&SPID1, 2, rxbuf);
    TP_CS_HIGH;

    y = rxbuf[0] << 4;
    y |= rxbuf[1] >> 4;

    return y;
}

__inline uint16_t lld_tpReadZ(void) {
	return 0;
}

#endif
