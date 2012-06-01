#include "touchpad.h"

static void spicb(SPIDriver *spip);
static const SPIConfig spicfg = {
	NULL,
	GPIOC,
	TP_CS,
	SPI_CR1_SPE | SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

void tpInit(void) {
	spiStart(&SPID1, &spicfg);	
}

void tpWriteData(uint8_t data) {
	uint16_t tx = 0xAA;

	SET_CS(0);
	spiSend(&SPID1, 1, &tx);
	SET_CS(1);
}

uint16_t tpReadData(void) {

}

uint16_t tpReadX(void) {
	uint8_t txbuf[1];
	uint8_t rxbuf[2];
	uint16_t x;

	txbuf[0] = 0x90;
	SET_CS(0);
	spiSend(&SPID1, 1, txbuf);
	spiReceive(&SPID1, 2, rxbuf);
	SET_CS(1);

	x = rxbuf[0] << 4;
	x |= rxbuf[1] >> 4;

	return x;
}

uint16_t tpReadY(void) {
	uint8_t txbuf[1];
    uint8_t rxbuf[2];
    uint16_t y;

    txbuf[0] = 0xd0;
    SET_CS(0);
    spiSend(&SPID1, 1, txbuf);
    spiReceive(&SPID1, 2, rxbuf);
    SET_CS(1);

    y = rxbuf[0] << 4;
    y |= rxbuf[1] >> 4;

    return y;
}

