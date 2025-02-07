#include <inttypes.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_flash.h"
#include "esp_err.h"
#include "spi_flash_chip_generic.h"
#include "memspi_host_driver.h"

DRAM_ATTR static const char* TAG = "XMC_FLASH_FIX";

#define SR1_SR2_BUSY_MASK   0x01        // read only
#define SR1_SR2_WEL_MASK    0x02        // read only
#define SR1_SR2_BP0_MASK    0x04
#define SR1_SR2_BP1_MASK    0x08
#define SR1_SR2_BP2_MASK    0x10
#define SR1_SR2_TB_MASK     0x20
#define SR1_SR2_SEC_MASK    0x40
#define SR1_SR2_SRP0_MASK   0x80        // one time programmable
#define SR1_SR2_SRP1_MASK   0x100       // one time programmable
#define SR1_SR2_QE_MASK     0x200
#define SR1_SR2_R_MASK      0x400
#define SR1_SR2_LB1_MASK    0x800       // one time programmable
#define SR1_SR2_LB2_MASK    0x1000      // one time programmable
#define SR1_SR2_LB3_MASK    0x2000      // one time programmable
#define SR1_SR2_CMP_MASK    0x4000
#define SR1_SR2_SUS_MASK    0x8000      // read only

IRAM_ATTR static bool read_sr(esp_flash_t* chip, uint32_t* out_sr);
IRAM_ATTR static bool write_sr(esp_flash_t* chip, uint32_t in_sr);

IRAM_ATTR void xmc_flash_fix(void* arg) 
{
    // Initialize SPI Flash for access to chip information
    esp_flash_t *chip = esp_flash_default_chip;
    if (chip == NULL) 
    {
        ESP_DRAM_LOGE(TAG, "Could not access flash module");
        return;
    }

    uint32_t flash_id = 0;
    esp_err_t ret = esp_flash_read_id(chip, &flash_id);

    if (ret == ESP_OK) 
        ESP_DRAM_LOGI(TAG, "Flash-ID: 0x%06" PRIx32, flash_id);
    else 
        ESP_DRAM_LOGI(TAG, "Could not read Flash-ID: %s", esp_err_to_name(ret));

    // Go ahead for XMC chips only
    if (((flash_id >> 16) & 0xff) != 0x20) 
    {
        ESP_DRAM_LOGI(TAG, "No XMC Flash Chip detected.");
        return;
    }

    // Check if SRP Bits are set
    uint32_t read_status = 0;
    if (!read_sr(chip, &read_status) || ((read_status & 0x0300) == 0x0300)) return;

    assert(read_status == 0x0200);

    // set write enable bit to set status bits
    if(memspi_host_set_write_protect(chip->host, false) != ESP_OK) return;

    //! define wanted status (0x380)
    uint32_t write_status = ( SR1_SR2_QE_MASK | SR1_SR2_SRP0_MASK | SR1_SR2_SRP1_MASK );
    if (!write_sr(chip, write_status)) return;

    do {       // check BUSY-Bit and wait
        if (!read_sr(chip, &read_status)) return;
    } while((read_status & (1 << 0)));

    assert(read_status == 0x0380);
}

static bool read_sr(esp_flash_t* chip, uint32_t* out_sr) 
{
    esp_err_t ret;

    *out_sr = 0;
    ret = spi_flash_common_read_status_16b_rdsr_rdsr2(chip, out_sr);

    if (ret != ESP_OK) 
    { 
        ESP_DRAM_LOGE(TAG, "Could not read status both register");
        return false;
    }

    ESP_DRAM_LOGI(TAG, "Read status register 1 and 2: 0x%04" PRIx32, *out_sr);
    return true;
}

static bool write_sr(esp_flash_t* chip, uint32_t in_sr) 
{
    esp_err_t ret;

    ret = spi_flash_common_write_status_16b_wrsr(chip, in_sr);

    if (ret != ESP_OK) 
    {
        ESP_DRAM_LOGE(TAG, "Could not write both status register");
        return false;
    }

    ESP_DRAM_LOGI(TAG, "Wrote both status register with 0x%04" PRIx32, in_sr);
    return true;
}