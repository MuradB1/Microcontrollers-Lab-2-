#include "ble_adv.h"

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>

/*
 * Device name comes from prj.conf:
 */
#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/*
 * TX power value included in advertising data.
 */
static const int8_t tx_power = 4;

/*
 * Advertising data.
 */
static const struct bt_data adv_data[] = {
    /*
     * BLE-only mode.
     */
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),

    /*
     * Complete local name.
     * This makes "Nizami_nRF52840" visible in nRF Connect.
     */
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),

    /*
     * TX power field included in advertising packet.
     */
    BT_DATA(BT_DATA_TX_POWER, &tx_power, sizeof(tx_power)),
};

/*
 * Optional scan response data.
 *
 * Keep empty for now.
 * If your device name becomes too long for the advertising packet,
 * you can move the name into scan response later.
 */
static const struct bt_data scan_response_data[] = {
    /*
     * Empty for this task.
     */
};

int ble_adv_init(void)
{
    int err;

    printk("BLE: initializing Bluetooth stack...\n");

    /*
     * Step 1:
     * Enable Bluetooth stack.
     *
     * This is the nRF52840/Zephyr equivalent of initializing BLE.
     */
    err = bt_enable(NULL);

    if (err) {
        printk("BLE: bt_enable failed: %d\n", err);
        return err;
    }

    printk("BLE: Bluetooth stack initialized\n");

    /*
     * Step 2:
     * Start advertising.
     */
    err = ble_adv_start();

    if (err) {
        printk("BLE: advertising start failed: %d\n", err);
        return err;
    }

    printk("BLE: advertising started\n");
    printk("BLE: device name = %s\n", DEVICE_NAME);
    printk("BLE: TX power field = %d dBm\n", tx_power);

    return 0;
}

int ble_adv_start(void)
{
    int err;

    /*
     * BT_LE_ADV_NCONN:
     *
     * Non-connectable advertising.
     * This means:
     * - the device appears in scan list
     * - no connection is accepted
     * - no GATT services are needed
     * - no sensor service is needed
     */
    err = bt_le_adv_start(
        BT_LE_ADV_NCONN,
        adv_data,
        ARRAY_SIZE(adv_data),
        scan_response_data,
        ARRAY_SIZE(scan_response_data)
    );

    return err;
}

int ble_adv_stop(void)
{
    int err;

    err = bt_le_adv_stop();

    if (err) {
        printk("BLE: failed to stop advertising: %d\n", err);
        return err;
    }

    printk("BLE: advertising stopped\n");

    return 0;
}
