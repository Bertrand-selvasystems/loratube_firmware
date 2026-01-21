#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "xfer_i2c.h"   // i2c_xfer_t, i2c_to_t

#define MAX_WRITE_LENGTH 128

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t addr7;          // adresse I2C 7-bit
    i2c_to_t to;              // timeout I2C par défaut
    const i2c_xfer_t *xfer; // interface transport injectée
    bool init_valid;
} fram_t;


// Initialisation du module FRAM (appel unique avant usage)
esp_err_t fram_init(fram_t *dev, const i2c_xfer_t *xfer, uint8_t addr7, i2c_to_t to);

// Lecture/écriture brut de données
esp_err_t fram_read(fram_t *dev, uint16_t addr, void *buf, size_t len, i2c_to_t to);
esp_err_t fram_write(fram_t *dev, uint16_t addr, const void *buf, size_t len, i2c_to_t to);

// Refresh du cache interne (si implémenté)
esp_err_t fram_refresh(fram_t *dev, i2c_to_t to);


#ifdef __cplusplus
}
#endif

