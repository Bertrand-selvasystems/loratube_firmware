#include "module_pca9536.h"

#define PCA9536_REG_INPUT    0x00
#define PCA9536_REG_OUTPUT   0x01
#define PCA9536_REG_POLARITY 0x02
#define PCA9536_REG_CONFIG   0x03

#define PCA9536_MASK4 0x0F

static inline bool pin_ok(pca9536_pin_t pin) {
    return (pin >= PCA9536_P0) && (pin <= PCA9536_P3);
}

static inline uint8_t pin_mask(pca9536_pin_t pin) {
    return (uint8_t)(1u << (uint8_t)pin);
}

static esp_err_t rd8(pca9536_t *dev, uint8_t reg, uint8_t *val, i2c_to_t to)
{
    if (!dev || !dev->xfer || !dev->xfer->write_read || !val) return ESP_ERR_INVALID_ARG;

    // PCA : write = reg, then read = 1 byte
    uint8_t w = reg;
    uint8_t r = 0;
    esp_err_t err = dev->xfer->write_read(dev->xfer->ctx, dev->addr7, &w, 1, &r, 1, to);
    if (err != ESP_OK) return err;

    *val = r;
    return ESP_OK;
}

static esp_err_t wr8(pca9536_t *dev, uint8_t reg, uint8_t val, i2c_to_t to)
{
    if (!dev || !dev->xfer || !dev->xfer->write) return ESP_ERR_INVALID_ARG;

    uint8_t buf[2] = { reg, val };
    return dev->xfer->write(dev->xfer->ctx, dev->addr7, buf, (uint16_t)sizeof(buf), to);
}

esp_err_t pca9536_read_reg(pca9536_t *dev, uint8_t reg, uint8_t *val, i2c_to_t to)
{
    return rd8(dev, reg, val, to);
}

esp_err_t pca9536_write_reg(pca9536_t *dev, uint8_t reg, uint8_t val, i2c_to_t to)
{
    return wr8(dev, reg, val, to);
}

esp_err_t pca9536_init(pca9536_t *dev, const i2c_xfer_t *xfer, uint8_t addr7, i2c_to_t to)
{
    if (!dev || !xfer) return ESP_ERR_INVALID_ARG;
    if (!xfer->write || !xfer->write_read) return ESP_ERR_INVALID_ARG;

    dev->xfer = xfer;
    dev->addr7 = addr7;

    dev->out_cache = 0;
    dev->cfg_cache = 0x0F; // par défaut : inputs
    dev->pol_cache = 0x00;
    dev->caches_valid = false;

    // Option : lire l’état réel du chip pour initialiser les caches
    return pca9536_refresh(dev, to);
}

esp_err_t pca9536_refresh(pca9536_t *dev, i2c_to_t to)
{
    if (!dev) return ESP_ERR_INVALID_ARG;

    uint8_t v = 0;
    esp_err_t err;

    err = rd8(dev, PCA9536_REG_OUTPUT, &v, to);
    if (err != ESP_OK) return err;
    dev->out_cache = (uint8_t)(v & PCA9536_MASK4);

    err = rd8(dev, PCA9536_REG_CONFIG, &v, to);
    if (err != ESP_OK) return err;
    dev->cfg_cache = (uint8_t)(v & PCA9536_MASK4);

    err = rd8(dev, PCA9536_REG_POLARITY, &v, to);
    if (err != ESP_OK) return err;
    dev->pol_cache = (uint8_t)(v & PCA9536_MASK4);

    dev->caches_valid = true;
    return ESP_OK;
}

esp_err_t pca9536_apply_output(pca9536_t *dev, i2c_to_t to)
{
    if (!dev) return ESP_ERR_INVALID_ARG;
    return wr8(dev, PCA9536_REG_OUTPUT, (uint8_t)(dev->out_cache & PCA9536_MASK4), to);
}

esp_err_t pca9536_apply_config(pca9536_t *dev, i2c_to_t to)
{
    if (!dev) return ESP_ERR_INVALID_ARG;
    return wr8(dev, PCA9536_REG_CONFIG, (uint8_t)(dev->cfg_cache & PCA9536_MASK4), to);
}

esp_err_t pca9536_apply_polarity(pca9536_t *dev, i2c_to_t to)
{
    if (!dev) return ESP_ERR_INVALID_ARG;
    return wr8(dev, PCA9536_REG_POLARITY, (uint8_t)(dev->pol_cache & PCA9536_MASK4), to);
}

esp_err_t pca9536_set_pin_dir(pca9536_t *dev, pca9536_pin_t pin, bool is_output, i2c_to_t to, bool apply)
{
    if (!dev || !pin_ok(pin)) return ESP_ERR_INVALID_ARG;

    uint8_t m = pin_mask(pin);

    // CONFIG: 1=input, 0=output
    if (is_output) dev->cfg_cache = (uint8_t)(dev->cfg_cache & ~m);
    else           dev->cfg_cache = (uint8_t)(dev->cfg_cache |  m);

    dev->cfg_cache &= PCA9536_MASK4;
    dev->caches_valid = true;

    return apply ? pca9536_apply_config(dev, to) : ESP_OK;
}

esp_err_t pca9536_set_pin_level(pca9536_t *dev, pca9536_pin_t pin, bool level, i2c_to_t to, bool apply)
{
    if (!dev || !pin_ok(pin)) return ESP_ERR_INVALID_ARG;

    uint8_t m = pin_mask(pin);

    if (level) dev->out_cache = (uint8_t)(dev->out_cache |  m);
    else       dev->out_cache = (uint8_t)(dev->out_cache & ~m);

    dev->out_cache &= PCA9536_MASK4;
    dev->caches_valid = true;

    return apply ? pca9536_apply_output(dev, to) : ESP_OK;
}

esp_err_t pca9536_write_outputs_masked(pca9536_t *dev, uint8_t mask4, uint8_t value4, i2c_to_t to, bool apply)
{
    if (!dev) return ESP_ERR_INVALID_ARG;

    mask4  &= PCA9536_MASK4;
    value4 &= PCA9536_MASK4;

    dev->out_cache = (uint8_t)((dev->out_cache & ~mask4) | (value4 & mask4));
    dev->out_cache &= PCA9536_MASK4;
    dev->caches_valid = true;

    return apply ? pca9536_apply_output(dev, to) : ESP_OK;
}

esp_err_t pca9536_read_inputs(pca9536_t *dev, uint8_t *in4, i2c_to_t to)
{
    if (!dev || !in4) return ESP_ERR_INVALID_ARG;

    uint8_t v = 0;
    esp_err_t err = rd8(dev, PCA9536_REG_INPUT, &v, to);
    if (err != ESP_OK) return err;

    *in4 = (uint8_t)(v & PCA9536_MASK4);
    return ESP_OK;
}

esp_err_t pca9536_get_pin_input(pca9536_t *dev, pca9536_pin_t pin, bool *level, i2c_to_t to)
{
    if (!dev || !level || !pin_ok(pin)) return ESP_ERR_INVALID_ARG;

    uint8_t in4 = 0;
    esp_err_t err = pca9536_read_inputs(dev, &in4, to);
    if (err != ESP_OK) return err;

    *level = ((in4 >> (uint8_t)pin) & 0x1u) ? true : false;
    return ESP_OK;
}
