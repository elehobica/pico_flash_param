# User flash parameter library for Raspberry Pi Pico

## Overview
* Store user parameters at the end part of flash memory of rp2040
* Allow parameter to have one type out of various primitive types
* Provide default value for factory reset
* Provide flash address auto calculation, otherwise allow to designate arbitary flash address
* Serve direct access setter and getter linked to the parameter's type
* Also serve access setter and getter by id, which requires to select the correct type

## Usage
### User parameter class declaration
* Prepare interherited class from FlashParamNs::FlashParam as Singleton (e.g. _ConfigParam_)
* Define user parameters with template with primitive type
  * Supported types: bool, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, and std::string
```
#pragma once

#include "FlashParam.h"

struct ConfigParam : FlashParamNs::FlashParam {
    static ConfigParam& instance()  // Singleton
    {
        static ConfigParam instance;
        return instance;
    }
    static constexpr uint32_t ID_BASE = FlashParamNs::CFG_ID_BASE;
    // Parameter<T>                      instance      id            name          default    size
    FlashParamNs::Parameter<std::string> P_CFG_STRING {ID_BASE + 0,  "CFG_STRING", "abcdefg", 16};  // std::string needs size
    FlashParamNs::Parameter<bool>        P_CFG_BOOL   {ID_BASE + 1,  "CFG_BOOL",   false};
    FlashParamNs::Parameter<uint8_t>     P_CFG_UINT8  {ID_BASE + 2,  "CFG_UINT8",  23};
    FlashParamNs::Parameter<uint16_t>    P_CFG_UINT16 {ID_BASE + 3,  "CFG_UINT16", 4096};
    FlashParamNs::Parameter<uint32_t>    P_CFG_UINT32 {ID_BASE + 4,  "CFG_UINT32", 65535*4};
    FlashParamNs::Parameter<uint64_t>    P_CFG_UINT64 {ID_BASE + 5,  "CFG_UINT64", 1ULL<<40};
    FlashParamNs::Parameter<int8_t>      P_CFG_INT8   {ID_BASE + 6,  "CFG_INT8",   -16};
    FlashParamNs::Parameter<int16_t>     P_CFG_INT16  {ID_BASE + 7,  "CFG_INT16",  -2047};
    FlashParamNs::Parameter<int32_t>     P_CFG_INT32  {ID_BASE + 8,  "CFG_INT32",  -65536*5};
    FlashParamNs::Parameter<int64_t>     P_CFG_INT64  {ID_BASE + 9,  "CFG_INT64",  -(1LL<<35)};
    FlashParamNs::Parameter<float>       P_CFG_FLOAT  {ID_BASE + 10, "CFG_FLOAT",  3.326f};
    FlashParamNs::Parameter<double>      P_CFG_DOUBLE {ID_BASE + 11, "CFG_DOUBLE", -1.056e-8};
};
```
### Instantiation
* _ConfigParam_ is singleton
```
ConfigParam cfgParam& = ConfigParam.instance();
```
### Initialize
* Load parameters from flash
* If flash is blank or flash format seems changed, default value are loaded
```
cfgParam.initialize();
```
### Finalize
* Store parameters to flash
```
cfgParam.finalize();
```
### Load default value
* Force to load default value
```
cfgParam.loadDefault();
```
### Print info
* Print parameter information
```
cfgParam.printInfo();
```
* Output example
```
=== UserFlash ===
FlashSize: 0x200000 (2097152)
UserReqSize: 0x400 (1024)
EraseSize: 0x1000 (4096)
PagePgrSize: 0x400 (1024)
UserFlashOfs: 0x1ff000 (2093056)
=== FlashParam ===
0x0000 CFG_MAP_HASH: 44081d (0xac31)
0x0004 CFG_STORE_COUNT: 67d (0x43)
0x0008 CFG_STRING: abcdefg
0x0018 CFG_BOOL: false
0x0019 CFG_UINT8: 23d (0x17)
0x001a CFG_UINT16: 4096d (0x1000)
0x001c CFG_UINT32: 262140d (0x3fffc)
0x0020 CFG_UINT64: 1099511627776d (0x10000000000)
0x0028 CFG_INT8: 240d (0xf0)
0x0029 CFG_INT16: -2047d (0xf801)
0x002b CFG_INT32: -327680d (0xfffb0000)
0x002f CFG_INT64: -34359738368d (0xfffffff800000000)
0x0037 CFG_FLOAT:  3.3260 (3.3260e+00)
0x003b CFG_DOUBLE: -0.0000 (-1.0560e-08)
```
### Getter/Setter by direct instance access
* Direct access available without designating its type
* For example, _value_ becomes `uint16_t` at following case
```
cfgParam.P_CFG_UINT16.set(0x89abcdef);
const auto& value = cfgParam.P_CFG_UINT16.get();
```
### Getter/Setter by id access
* To access by id, template type needs to be designated to meet the type of the parameter
```
cfgParam.setValue<uint16_t>(cfgParam.ID_BASE + 3, 0x0123);
const auto& value = cfgParam.getValue<uint16_t>(cfgParam.ID_BASE + 3);
```

## Operating with multicore program
* As generaal, flash operation should be done from core0 only
* Even in that case, `flash_safe_execute_core_init()` needs to be called from core1 to notify safe condition for programming flash 
* Otherwise, `cfgParam.finalize()` will fail due to the faulure of `UserFlash::program()`
* Refer to samples/multicore_test project
```
...
#include "pico/flash.h"
#include "pico/multicore.h"

static void core1_process() {
    flash_safe_execute_core_init();  // notify core0 that there's no access to flash on core1
    ...
}

int main() {
    ...
    multicore_launch_core1(core1_process);
    ...
}

```

## How to build sample projects
* See ["Getting started with Raspberry Pi Pico"](https://datasheets.raspberrypi.org/pico/getting-started-with-pico.pdf)
* Put "pico-sdk", "pico-examples" and "pico-extras" on the same level with this project folder.
* Set environmental variables for PICO_SDK_PATH, PICO_EXTRAS_PATH and PICO_EXAMPLES_PATH
* Confirmed with Pico SDK 1.5.1
```
> git clone -b 1.5.1 https://github.com/raspberrypi/pico-sdk.git
> cd pico-sdk
> git submodule update -i
> cd ..
> git clone -b sdk-1.5.1 https://github.com/raspberrypi/pico-examples.git
>
> git clone -b sdk-1.5.1 https://github.com/raspberrypi/pico-extras.git
> 
> git clone -b main https://github.com/elehobica/pico_user_flash.git
> cd pico_user_flash
> cd samples\xxxxx  # target sample project
> cd ..
```
### Windows
* Build is confirmed with Developer Command Prompt for VS 2022 and Visual Studio Code on Windows environment
* Confirmed with cmake-3.27.2-windows-x86_64 and gcc-arm-none-eabi-10.3-2021.10-win32
* Lanuch "Developer Command Prompt for VS 2022"
```
> mkdir build && cd build
> cmake -G "NMake Makefiles" ..
> nmake
```
* Put "*.uf2" on RPI-RP2 drive
### Linux
* Build is confirmed with [rp2040-dev-docker:sdk-1.5.1-1.0.1]( https://hub.docker.com/r/elehobica/rp2040-dev-docker) 
* Confirmed with cmake-3.22.1 and arm-none-eabi-gcc (15:10.3-2021.07-4) 10.3.1
```
$ mkdir build && cd build
$ cmake ..
$ make -j4
```
* Download "*.uf2" on RPI-RP2 drive
