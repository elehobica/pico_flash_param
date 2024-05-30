# User flash parameter library for Raspberry Pi Pico

## Overview
* Store user parameters at the end part of flash memory of rp2040
* Allow parameters to have one type out of various primitive types
* Provide default value for factory reset
* Provide flash address auto calculation, otherwise allow to designate arbitary flash address
* Serve Flash parameter direct access setter and getter with its type
* Also serve access setter and getter by id, which requires to select correct type

## Usage
### User parameter class declaration
* Generate interherited class from FlashParamNs::FlashParam as Singleton
* Define user parameters with template with primitive type
  * Supported types: bool, uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, const char* and std::string
```
#include "FlashParam.h"
struct ConfigParam : FlashParamNs::FlashParam {
    static ConfigParam& instance()  // Singleton
    {
        static ConfigParam instance;
        return instance;
    }
    // Parameter<T>                      inst     id                              name     default    size
    FlashParamNs::Parameter<uint32_t>    P_PARAM0{FlashParamNs::CFG_ID_BASE + 0, "param0", 10};
    FlashParamNs::Parameter<uint8_t>     P_PARAM1{FlashParamNs::CFG_ID_BASE + 1, "param1", 240};
    FlashParamNs::Parameter<int16_t>     P_PARAM2{FlashParamNs::CFG_ID_BASE + 2, "param2", -4096};
    FlashParamNs::Parameter<float>       P_PARAM3{FlashParamNs::CFG_ID_BASE + 3, "param3", 1.23e-3};
    FlashParamNs::Parameter<const char*> P_PARAM4{FlashParamNs::CFG_ID_BASE + 4, "param4", "ABCD"};
    FlashParamNs::Parameter<std::string> P_PARAM5{FlashParamNs::CFG_ID_BASE + 5, "param5", "0123456", 16};  // need to designate size for std::string
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
0x0000 CFG_TOTAL_SUM: 0d (0x0)
0x0004 CFG_BOOT_COUNT: 0d (0x0)
0x0008 CFG_REVISION: 1.0.0
0x0010 CFG_STRING: abcdefg
0x0020 CFG_BOOL: false
0x0021 CFG_UINT8: 23d (0x17)
0x0022 CFG_UINT16: 4096d (0x1000)
0x0024 CFG_UINT32: 262140d (0x3fffc)
0x0028 CFG_UINT64: 1099511627776d (0x10000000000)
0x0030 CFG_INT8: 240d (0xf0)
0x0031 CFG_INT16: -2047d (0xf801)
0x0033 CFG_INT32: -327680d (0xfffb0000)
0x0037 CFG_INT64: -34359738368d (0xfffffff800000000)
0x003f CFG_FLOAT:  3.3260 (3.3260e+00)
0x0043 CFG_DOUBLE: -0.0000 (-1.0560e-08)
```
### Getter/Setter by direct instance access
* Direct access available without designating its type
* In following case, _value_ gets `uint32_t`
```
cfgParam.P_PARAM0.set(0x89abcdef);
auto value = cfgParam.P_PARAM0.get();
```
### Getter/Setter by id access
* For access by id, template type needs to be designated by the type of value
```
cfgParam.setValue<uint32_t>(0, 0x89abcdef);
auto value = cfgParam.getValue<uint32_t>();
```
