/**
  @file main.c
  @brief Free RTOS with D2XX
  An example of thread-safe D2XX library usage with FreeRTOS
 */
/*
 * ============================================================================
 * (C) Copyright,  Bridgetek Pte. Ltd.
 * ============================================================================
 *
 * This source code ("the Software") is provided by Bridgetek Pte Ltd
 * ("Bridgetek") subject to the licence terms set out
 * http://brtchip.com/BRTSourceCodeLicenseAgreement/ ("the Licence Terms").
 * You must read the Licence Terms before downloading or using the Software.
 * By installing or using the Software you agree to the Licence Terms. If you
 * do not agree to the Licence Terms then do not download or use the Software.
 *
 * Without prejudice to the Licence Terms, here is a summary of some of the key
 * terms of the Licence Terms (and in the event of any conflict between this
 * summary and the Licence Terms then the text of the Licence Terms will
 * prevail).
 *
 * The Software is provided "as is".
 * There are no warranties (or similar) in relation to the quality of the
 * Software. You use it at your own risk.
 * The Software should not be used in, or for, any medical device, system or
 * appliance. There are exclusions of FTDI liability for certain types of loss
 * such as: special loss or damage; incidental loss or damage; indirect or
 * consequential loss or damage; loss of income; loss of business; loss of
 * profits; loss of revenue; loss of contracts; business interruption; loss of
 * the use of money or anticipated savings; loss of information; loss of
 * opportunity; loss of goodwill or reputation; and/or loss of, damage to or
 * corruption of data.
 * There is a monetary cap on FTDI's liability.
 * The Software may have subsequently been amended by another user and then
 * distributed by that other user ("Adapted Software").  If so that user may
 * have additional licence terms that apply to those amendments. However, FTDI
 * has no liability in relation to those amendments.
 * ============================================================================
 */

/* INCLUDES ************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include <EVE.h>

#include "eve_example.h"


/* CONSTANTS ***********************************************************************/

/**
 @brief Storage namespace in NV memory for touchscreen calibration
 values.
 */
#define STORAGE_NAMESPACE "touchscreen"
#define STORAGE_CALIBRATION "calibration"

/* LOCAL FUNCTIONS / INLINES *******************************************************/

void setup(void);

/* FUNCTIONS ***********************************************************************/

/**
 * @brief Functions used to store calibration data in flash.
 */
//@{
int8_t platform_calib_init(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    return 0;
}

int8_t platform_calib_write(struct touchscreen_calibration *calib)
{
    nvs_handle my_handle;
    esp_err_t err = ESP_OK;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Write value including previously saved blob if available
    err = nvs_set_blob(my_handle, STORAGE_CALIBRATION, calib, sizeof(struct touchscreen_calibration));

    if (err != ESP_OK) return err;

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) return err;

    // Close
    nvs_close(my_handle);
    return err;
}

int8_t platform_calib_read(struct touchscreen_calibration *calib)
{
    nvs_handle my_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Read calibration blob
    size_t required_size = sizeof(struct touchscreen_calibration);
    err = nvs_get_blob(my_handle, STORAGE_CALIBRATION, calib, &required_size);
    if (err != ESP_OK) return err;
    if (required_size == 0) err = -1;

    // Close
    nvs_close(my_handle);
    return err;
}
//@}

void main_thread(void *p)
{
    ESP_LOGE(__FUNCTION__, "setup starting");
    /* Setup UART */
    setup();

    ESP_LOGE(__FUNCTION__, "example starting");
    /* Start example code */
    eve_example();

    // function never returns 
    for (;;) ;
}

void setup(void)
{
}

void app_main()
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    xTaskCreate(&main_thread, "main", 4096, NULL, 5, NULL);
}
