#include "module_fram.h"
#include <string.h>

esp_err_t fram_init(fram_t *dev, const i2c_xfer_t *xfer, uint8_t addr7, i2c_to_t to)
{
    if (!dev || !xfer) return ESP_ERR_INVALID_ARG;
    if (!xfer->write || !xfer->write_read) return ESP_ERR_INVALID_ARG;

    dev->xfer = xfer;
    dev->addr7 = addr7;
    dev->init_valid = false;

    return fram_refresh(dev, to);
}


esp_err_t fram_read(fram_t *dev, uint16_t addr, void *buf, size_t len, i2c_to_t to)
{
    if (!dev || !dev->xfer || !dev->xfer->write_read || !buf) return ESP_ERR_INVALID_ARG;
    if (len == 0) return ESP_OK;

    uint8_t a[2] = { (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF) };
    return dev->xfer->write_read(dev->xfer->ctx, dev->addr7, a, 2, (uint8_t*)buf, (uint16_t)len, to);
}

esp_err_t fram_write(fram_t *dev, uint16_t addr, const void *buf, size_t len, i2c_to_t to)
{
    if (!dev || !dev->xfer || !dev->xfer->write || (!buf && len)) return ESP_ERR_INVALID_ARG;
    if (len == 0) return ESP_OK;

    // Ici version “petit write”. Pour du gros, chunks.
    if (len > MAX_WRITE_LENGTH) return ESP_ERR_INVALID_SIZE; // garde-fou arbitraire

    uint8_t tmp[2 + MAX_WRITE_LENGTH];
    tmp[0] = (uint8_t)(addr >> 8);
    tmp[1] = (uint8_t)(addr & 0xFF);
    memcpy(&tmp[2], buf, len);

    return dev->xfer->write(dev->xfer->ctx, dev->addr7, tmp, (uint16_t)(len + 2), to);
}

//(fram_t *dev, uint8_t reg, uint8_t *val, i2c_to_t to)
esp_err_t fram_refresh(fram_t *dev, i2c_to_t to)
{
    if (!dev) return ESP_ERR_INVALID_ARG;

    uint8_t old;
    uint8_t v = 50;
    uint8_t u = 0;
    esp_err_t err;

    err = fram_read(dev, 0, &old, 1, to);
    if (err != ESP_OK) return err;

    err = fram_write(dev, 0, &v, 1, to);
    if (err != ESP_OK) return err;

    err = fram_read(dev, 0, &u, 1, to);
    if (err != ESP_OK) return err;

    if (v != u) return ESP_ERR_INVALID_RESPONSE;

    for (int i = 0; i < 3; i++)
    {
        err = fram_read(dev, 0, &u, 1, to);
        if (err != ESP_OK) return err;
        if (u != v) return ESP_ERR_INVALID_RESPONSE;
    }

    err = fram_write(dev, 0, &old, 1, to);
    if (err != ESP_OK) return err;

    dev->init_valid = true;
    return ESP_OK;
}