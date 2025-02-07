# bricked-esp32-xmc-flash-fix
Workaround for esp-idf issue #7994 using esp-idf v5.4.

Espressif uses flash chips from various manufacturers, including XMC, in its ESP32 modules. Although there have been no problems with the chips from other manufacturers to date, with XMC chips it could happen that garbage data was written to the flash, which in particular set the status registers incorrectly. If the permanent write protection of the status registers was set, the ESP32 became unusable. This issue is described just for the chips with device code 0x4018, nevertheless Espressif gives the opportunity to exclude all XMC-Chips identified by the manufacturer code 0x20. The problem manifested itself with an unterminating boot loop giving the following error message:

rst:0x10 (RTCWDT_RTC_RESET),boot:0x33 (SPI_FAST_FLASH_BOOT)

configsip: 0, SPIWP:0xee

clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00

mode:DIO, clock div:2

load:0x3fff0030,len:380

ho 0 tail 12 room 4

load:0x07800000,len:3378177


The problem has already been described as an issue at espressif: https://github.com/espressif/esp-idf/issues/7994

Workarounds have already been published online. However, the esp-idf in a version < v5.x was used for this, and difficulties with watchdog resets were described in some cases.
However, I have rebuilt the workaround with the esp-idf in version 5.4, which has resulted in cleaner and cleaner code. The code is also stable.

The program was tested for ESP32-WROVER Rev1 to Rev3. It checks if the chip is from XMC. Just in case it is and if the SRP-Bits are not set the flash registers are written.

HINT:
If you want to use this workaround, make sure to use the correct status register values for your project!
