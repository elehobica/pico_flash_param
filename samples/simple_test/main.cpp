/*------------------------------------------------------/
/ Copyright (c) 2021, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/------------------------------------------------------*/

#include <cstdio>

#include "pico/stdlib.h"
#include "ConfigParam.h"

static inline uint32_t _millis()
{
    return to_ms_since_boot(get_absolute_time());
}

int main() {
    stdio_init_all();

    ConfigParam& cfgParam = ConfigParam::instance();

    // serial connection waiting (max 1 sec)
    while (!stdio_usb_connected() && _millis() < 1000) {
        sleep_ms(100);
    }
    printf("\r\n");

    cfgParam.printInfo();
    cfgParam.P_CFG_REVISION.set("1.0.1");
    cfgParam.printInfo();

    while (true) {}

    return 0;
}
