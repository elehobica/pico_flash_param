/*------------------------------------------------------/
/ Copyright (c) 2024, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/------------------------------------------------------*/

#include <cstdio>
#include <execution>

#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "config_wifi.h"
#include "ConfigParam.h"

static bool picoW = false;
static constexpr uint PIN_LED = 25;  // PICO_DEFAULT_LED_PIN of Pico

static inline uint32_t _millis()
{
    return to_ms_since_boot(get_absolute_time());
}

static void _printHelp()
{
    printf("h: print Help\r\n");
    printf("w: configure Wi-Fi\r\n");
    printf("c: clear Flash\r\n");
}

static bool _check_pico_w()
{
    // from discussions from https://github.com/earlephilhower/arduino-pico/issues/849
    adc_init();
    auto dir = gpio_get_dir(29);
    auto fnc = gpio_get_function(29);
    adc_gpio_init(29);
    adc_select_input(3);
    auto adc29 = adc_read();
    gpio_set_function(29, fnc);
    gpio_set_dir(29, dir);

    dir = gpio_get_dir(25);
    fnc = gpio_get_function(25);
    gpio_init(25);
    gpio_set_dir(25, GPIO_IN);
    auto gp25 = gpio_get(25);
    gpio_set_function(25, fnc);
    gpio_set_dir(25, dir);

    if (gp25) {
        return true; // Can't tell, so assume yes
    } else if (adc29 < 200) {
        return true; // PicoW
    } else {
        return false;
    }
}

static bool _connect_wifi(const std::string& ssid, const std::string& password, const int retry = 3)
{
    printf("... connecting Wi-Fi\r\n");

    cyw43_arch_enable_sta_mode();
    for (int i = 0; i < retry; i++) {
        if (cyw43_arch_wifi_connect_timeout_ms(ssid.c_str(), password.c_str(), CYW43_AUTH_WPA2_AES_PSK, 10000)) {
            printf("failed to connect: %d\r\n", i);
            if (i < retry - 1) {
                continue;
            } else {
                return false;
            }
        }
    }
    printf("connected\r\n");

    return true;
}

static void _set_led(bool flag)
{
    if (picoW) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, flag);
    } else {
        gpio_put(PIN_LED, flag);
    }
}

int main() {
    stdio_init_all();
    picoW = _check_pico_w();

    ConfigParam& cfgParam = ConfigParam::instance();
    FlashParamNs::UserFlash& usrFlash = FlashParamNs::UserFlash::instance();

    // serial connection waiting (max 1 sec)
    while (!stdio_usb_connected() && _millis() < 1000) {
        sleep_ms(100);
    }
    printf("\r\n");

    cfgParam.initialize();

    // Pico / Pico W dependencies
    if (picoW) {
        if (cyw43_arch_init()) {  // this is needed for driving LED
            printf("cyw43 init failed\r\n");
            return 1;
        }
        printf("Pico W\r\n");

        const auto& ssid = cfgParam.P_CFG_WIFI_SSID.get();
        const auto& pass = cfgParam.P_CFG_WIFI_PASS.get();
        printf("SSID: %s\r\n", ssid.c_str());
        printf("PASS: %s\r\n", std::string(pass.length(), '*').c_str());

        if (ssid.length() > 0 && pass.length() > 0) {
            _set_led(true);
            if (_connect_wifi(ssid, pass)) {
                printf("SUCCESS: connected to %s\r\n", ssid.c_str());
            } else {
                printf("ERROR: failed Wi-Fi connection: %s\r\n", ssid.c_str());
            }
        } else {
            printf("No Wi-Fi SSID information. press 'w' to configure Wi-Fi\r\n");
        }
    } else {
        printf("ERROR: only Pico-W, not Pico, is supported\r\n");
        // LED
        gpio_init(PIN_LED);
        gpio_set_dir(PIN_LED, GPIO_OUT);
        return 1;
    }
    _set_led(false);

    while (true) {
        int chr;
        if ((chr = getchar_timeout_us(10000)) != PICO_ERROR_TIMEOUT) {
            char c = static_cast<char>(chr);
            if (c == 'h') {
                _printHelp();
            } else if (c == 'c') {
                if (usrFlash.clear()) {
                    printf("Flash cleared\r\n");
                } else {
                    printf("ERROR: failed to clear Flash\r\n");
                }
            } else if (c == 'w' && picoW) {
                std::string ssid, pass;
                if (config_wifi(ssid, pass)) {
                    // trial to connect Wi-Fi
                    if (_connect_wifi(ssid, pass)) {
                        printf("SUCCESS: connected to %s\r\n", ssid.c_str());
                        // store to flash
                        cfgParam.P_CFG_WIFI_SSID.set(ssid);
                        cfgParam.P_CFG_WIFI_PASS.set(pass);
                        if (cfgParam.finalize()) {
                            printf("Wi-Fi configuration stored to flash\r\n");
                        } else {
                            printf("ERROR: failed to store Wi-Fi configuration to flash\r\n");
                        }
                    } else {
                        printf("ERROR: failed Wi-Fi connection: %s\r\n", ssid.c_str());
                    }
                }
            }
        }
    }
    return 0;
}
