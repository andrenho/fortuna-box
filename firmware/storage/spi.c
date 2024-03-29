#include "spi.h"

#include <stdio.h>

#include <avr/pgmspace.h>
#include <avr/io.h>
#include <util/delay.h>

#if DEBUG_SDCARD >= 2
#  include "ansi.h"
#endif

void spi_init(void)
{
    DDRB |= (1 << DDB2) | (1 << DDB3) | (1 << DDB5);   // MOSI, SCK: output
    // PB2 (SS) must be set to output, otherwise SPI will not work
    PORTB |= (1 << PB2);
    
    // enable SPI, set as MASTER, clock to fosc/128 (TODO - can it go faster?)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0); // | (1 << SPI2X);
}

uint8_t spi_send(uint8_t byte)
{
    SPDR = byte;
    loop_until_bit_is_set(SPSR, SPIF);
    uint8_t r = SPDR;
#if DEBUG_SDCARD >= 2
    if (byte != 0xff)
        printf_P(PSTR(GRN "%02X:" RED "%02X "), byte, r);
    else
        printf_P(PSTR(RED "%02X "), r);
#endif
    return r;
}

uint8_t spi_recv(void)
{
    return spi_send(0xff);
}

uint8_t spi_recv_ignore_ff(void)
{
    uint8_t i = 0, r;
    while ((r = spi_recv()) == 0xff) {
        ++i;
        _delay_us(30);
        if (i > 8)
            break;  // timeout
    }
    return r;
}

// vim:ts=4:sts=4:sw=4:expandtab
