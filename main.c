#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

/* ── Данные температуры (25.00°C = 2500) ── */
static int16_t temperature_value = 2500;

/* ── Callback чтения температуры ── */
static ssize_t read_temperature(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr,
                                void *buf, uint16_t len,
                                uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             &temperature_value,
                             sizeof(temperature_value));
}

/* ── GATT сервис ── */
BT_GATT_SERVICE_DEFINE(sensor_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DECLARE_16(0x181A)),
    BT_GATT_CHARACTERISTIC(BT_UUID_DECLARE_16(0x2A6E),
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           read_temperature, NULL,
                           &temperature_value),
    BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

/* ── Advertising данные ── */
static const struct bt_data adv_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS,
                  BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
                  BT_UUID_16_ENCODE(0x181A)),
};

static const struct bt_data scan_rsp[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE,
            CONFIG_BT_DEVICE_NAME,
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

/* ── Запуск рекламы ── */
static void start_advertising(void)
{
    int err = bt_le_adv_start(BT_LE_ADV_PARAM(
                              BT_LE_ADV_OPT_CONN,
                              0x0320,   /* 500ms = 500/0.625 = 800 = 0x0320 */
                              0x0320,
                              NULL),
                          adv_data, ARRAY_SIZE(adv_data),
                          scan_rsp, ARRAY_SIZE(scan_rsp));
    if (err) {
        LOG_ERR("Advertising failed to start: %d", err);
        return;
    }
    LOG_INF("Advertising started");
}

/* ── Callbacks соединения ── */
static void on_connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Connection failed: %d", err);
        return;
    }
    LOG_INF("Device connected");
}

static void on_disconnected(struct bt_conn *conn, uint8_t reason)
{
    LOG_INF("Disconnected, reason: %d", reason);
    start_advertising();
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected    = on_connected,
    .disconnected = on_disconnected,
};

/* ── Главная функция ── */
int main(void)
{
    LOG_INF("nRF52840 Sensor Demo starting...");

    /* Шаг 1: инициализация BLE стека */
    int err = bt_enable(NULL);
    if (err) {
        LOG_ERR("BLE init failed: %d", err);
        return err;
    }
    LOG_INF("BLE enabled");

    /* Шаг 2: запуск рекламы */
    start_advertising();

    /* Шаг 3: главный цикл — обновление температуры */
    while (1) {
        k_sleep(K_SECONDS(2));

        temperature_value += 10;
        if (temperature_value > 4000) {
            temperature_value = 2000;
        }

        bt_gatt_notify(NULL,
                       &sensor_svc.attrs[2],
                       &temperature_value,
                       sizeof(temperature_value));

        LOG_DBG("Temp: %d.%02d C",
                temperature_value / 100,
                temperature_value % 100);
    }

    return 0;
}