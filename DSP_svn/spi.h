/* 
 * File:   spi.h
 * Author: Erik
 *
 * Created on den 22 december 2015, 17:26
 */

#ifndef SPI_H
#define	SPI_H


// =============================================================================
// Include statements
// =============================================================================
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// Public type definitions
// =============================================================================

typedef enum spi_device_t
{
    SPI_DEVICE_PCM1774_I2S,
    SPI_DEVICE_PIC32,
    SPI_DEVICE_PCM1774
} spi_device_t;

// =============================================================================
// Global variable declarations
// =============================================================================

// =============================================================================
// Global constatants
// =============================================================================

// =============================================================================
// Public function declarations
// =============================================================================

/**
 * @brief Initializes the SPI interface.
 * @param device - The device of which SPI interface should be initialized.
 * @return void
 */
void spi_init(spi_device_t device);

/**
 * @brief Closes the SPI interface for a specified device.
 * @param device - The device of which SPI interface should be closed.
 * @return void
 */
void spi_deinit(spi_device_t device);

/**
 * @brief Performs a transaction over the SPI interface.
 * @details This is a blocking operation.
 * @param device - The device to communicate with.
 * @param data - The data to send.
 * @return The reveived data.
 */
uint16_t spi_tranceive(spi_device_t device, uint16_t data);

/**
 * @brief Writes data over a specified SPI bus.
 * @details This function might return before all data has been sent.
 * @param device - The device to write to.
 * @param data - The data to send.
 * @return void
 */
void spi_write(spi_device_t device, uint16_t data);

/**
 * @brief Performs a spi transaction where 0 is sent and returns the received
 *        data.
 * @details This is a blocking operation.
 * @param device - The device to read from.
 * @return The received data.
 */
uint16_t spi_read(spi_device_t device);

/**
 * @brief Checks if the spi interface to a given device is busy.
 * @param device - The device which spi interface to check.
 * @return True if the spi interface was busy, false otherwise.
 */
bool spi_is_busy(spi_device_t device);

#endif	/* SPI_H */

