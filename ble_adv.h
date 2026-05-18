#ifndef BLE_ADV_H
#define BLE_ADV_H

/*
 * Initializes Bluetooth stack and starts advertising.
 *
 * Returns:
 *  0  - success
 * <0  - error code
 */
int ble_adv_init(void);

/*
 * Starts BLE advertising.
 */
int ble_adv_start(void);

/*
 * Stops BLE advertising.
 */
int ble_adv_stop(void);

#endif