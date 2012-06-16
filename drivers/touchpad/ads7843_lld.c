#include "ads7843_lld.h"

#ifdef TOUCHPAD_USE_ADS7843

__inline uint16_t lld_readX(void) {
    uint8_t txbuf[1];
    uint8_t rxbuf[2];
    uint16_t x;

    txbuf[0] = 0xd0;
    SET_CS(0);
    spiSend(&SPID1, 1, txbuf);
    spiReceive(&SPID1, 2, rxbuf);
    SET_CS(1);

    x = rxbuf[0] << 4;
    x |= rxbuf[1] >> 4;
    
    return x;
}

__inline uint16_t lld_readY(void) {
    uint8_t txbuf[1];
    uint8_t rxbuf[2];
    uint16_t y;

    txbuf[0] = 0x90;
    SET_CS(0);
    spiSend(&SPID1, 1, txbuf);
    spiReceive(&SPID1, 2, rxbuf);
    SET_CS(1);

    y = rxbuf[0] << 4;
    y |= rxbuf[1] >> 4;

    return y;
}

__inline uint16_t lld_readZ(void) {
	return 0;
}

#endif
