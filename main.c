#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "ble_adv.h"

int main(void)
{
    int err;

    printk("System started\n");

    /*
     * Initialize and start BLE advertising.
     * All BLE-specific logic is inside ble_adv.c.
     */
    err = ble_adv_init();

    if (err) {
        printk("BLE advertising initialization failed: %d\n", err);
        return 0;
    }

    printk("BLE advertising initialization successful\n");

    while (1) {
        /*
         * Advertising runs in the background after bt_le_adv_start().
         * Nothing else is needed here for this task.
         */
        k_sleep(K_SECONDS(1));
    }

    return 0;
}