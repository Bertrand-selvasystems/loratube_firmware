#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "xfer_i2c.h"   // i2c_xfer_t, i2c_to_t

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PCA9536_P0 = 0,
    PCA9536_P1 = 1,
    PCA9536_P2 = 2,
    PCA9536_P3 = 3,
} pca9536_pin_t;

typedef struct {
    const i2c_xfer_t *xfer; // interface transport injectée
    uint8_t addr7;          // adresse I2C 7-bit

    // --- caches (4 LSB utilisés) ---
    uint8_t out_cache;      // miroir du registre OUTPUT
    uint8_t cfg_cache;      // miroir du registre CONFIG (1=input, 0=output)
    uint8_t pol_cache;      // miroir du registre POLARITY

    bool caches_valid;
} pca9536_t;

// Init : branche xfer + adresse, et (option) lit l'état initial des registres
esp_err_t pca9536_init(pca9536_t *dev, const i2c_xfer_t *xfer, uint8_t addr7, i2c_to_t to);

// Lecture/écriture registres bruts (utile debug)
esp_err_t pca9536_read_reg(pca9536_t *dev, uint8_t reg, uint8_t *val, i2c_to_t to);
esp_err_t pca9536_write_reg(pca9536_t *dev, uint8_t reg, uint8_t val, i2c_to_t to);

// Refresh caches depuis le chip (OUTPUT/CONFIG/POLARITY)
esp_err_t pca9536_refresh(pca9536_t *dev, i2c_to_t to);

// Appliquer caches vers le chip
esp_err_t pca9536_apply_output(pca9536_t *dev, i2c_to_t to);
esp_err_t pca9536_apply_config(pca9536_t *dev, i2c_to_t to);
esp_err_t pca9536_apply_polarity(pca9536_t *dev, i2c_to_t to);

// API haut niveau
esp_err_t pca9536_set_pin_dir(pca9536_t *dev, pca9536_pin_t pin, bool is_output, i2c_to_t to, bool apply);
esp_err_t pca9536_set_pin_level(pca9536_t *dev, pca9536_pin_t pin, bool level, i2c_to_t to, bool apply);

// Écriture groupée (mask 4 bits)
esp_err_t pca9536_write_outputs_masked(pca9536_t *dev, uint8_t mask4, uint8_t value4, i2c_to_t to, bool apply);

// Lecture input (pin ou 4 bits)
esp_err_t pca9536_read_inputs(pca9536_t *dev, uint8_t *in4, i2c_to_t to);
esp_err_t pca9536_get_pin_input(pca9536_t *dev, pca9536_pin_t pin, bool *level, i2c_to_t to);

#ifdef __cplusplus
}
#endif
