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
    // Parameter<T>                      inst     id  name     default    size
    FlashParamNs::Parameter<uint32_t>    P_PARAM0{0, "param0", 10};
    FlashParamNs::Parameter<uint8_t>     P_PARAM1{1, "param1", 240};
    FlashParamNs::Parameter<int16_t>     P_PARAM2{2, "param2", -4096};
    FlashParamNs::Parameter<float>       P_PARAM3{3, "param3", 1.23e-3};
    FlashParamNs::Parameter<const char*> P_PARAM4{4, "param4", "ABCD"};
    FlashParamNs::Parameter<std::string> P_PARAM5{5, "param5", "0123456", 16};  // need to designate size for std::string
};
```
### Getter/Setter by direct instance access
* Direct access available without designating its type
```
ConfigParam cfgParam& = ConfigParam.instance();
cfgParam.P_PARAM0.set(0x89abcdef);
// following 'value' is uint32_t
auto value = cfgParam.P_PARAM0.get();
```
### Getter/Setter by id access
* Template type needs to match the type of value
```
ConfigParam cfgParam& = ConfigParam.instance();
cfgParam.setValue<uint32_t>(0, 0x89abcdef);
auto value = cfgParam.getValue<uint32_t>();
```
