/*------------------------------------------------------/
/ Copyright (c) 2024, Elehobica
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

static void _printHelp()
{
    printf("h: print help\r\n");
    printf("d: loadDefault\r\n");
    printf("f: finalize and exit\r\n");
    printf("p: printInfo\r\n");
    printf("1: change values 1\r\n");
    printf("2: change values 2\r\n");
}

int main() {
    stdio_init_all();

    ConfigParam& cfgParam = ConfigParam::instance();

    // serial connection waiting (max 1 sec)
    while (!stdio_usb_connected() && _millis() < 1000) {
        sleep_ms(100);
    }
    printf("\r\n");

    cfgParam.initialize();
    cfgParam.printInfo();

    while (true) {
        int chr;
        if ((chr = getchar_timeout_us(10000)) != PICO_ERROR_TIMEOUT) {
            char c = static_cast<char>(chr);
            if (c == 'h') {
                _printHelp();
            } else if (c == 'd') {
                cfgParam.loadDefault();
                printf("loadDefault\r\n");
            } else if (c == 'f') {
                if (cfgParam.finalize()) {
                    printf("success to store flash parameters\r\n");
                } else {
                    printf("failure to store flash parameters\r\n");
                }
            } else if (c == 'p') {
                cfgParam.printInfo();
            } else if (c == '1') {
                cfgParam.P_CFG_INT8.set(-10);
                cfgParam.P_CFG_STRING.set("abcdef0123456789ABCDEF");
            } else if (c == '2') {
                cfgParam.P_CFG_INT8.set(3);
                cfgParam.P_CFG_STRING.set("0123456789");
            }
        }
    }

    return 0;
}
