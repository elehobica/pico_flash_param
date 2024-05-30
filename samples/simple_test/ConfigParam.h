/*-----------------------------------------------------------/
/ ConfigParam.h
/------------------------------------------------------------/
/ Copyright (c) 2024, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/-----------------------------------------------------------*/

#pragma once

#include "FlashParam.h"

typedef enum {
    CFG_REVISION = FlashParamNs::CFG_ID_BASE,
    CFG_STRING,
    CFG_BOOL,
    CFG_UINT8,
    CFG_UINT16,
    CFG_UINT32,
    CFG_UINT64,
    CFG_INT8,
    CFG_INT16,
    CFG_INT32,
    CFG_INT64,
    CFG_FLOAT,
    CFG_DOUBLE,
} ParamId_t;

//=================================
// Interface of ConfigParam class
//=================================
struct ConfigParam : FlashParamNs::FlashParam {
    static ConfigParam& instance() {  // Singleton
        static ConfigParam instance;
        return instance;
    }

    // Parameter<T>                      instance         id               name             default    size
    FlashParamNs::Parameter<const char*> P_CFG_REVISION  {CFG_REVISION,   "CFG_REVISION",   "1.0.0",   8};
    FlashParamNs::Parameter<std::string> P_CFG_STRING    {CFG_STRING,     "CFG_STRING",     "abcdefg", 16};
    FlashParamNs::Parameter<bool>        P_CFG_BOOL      {CFG_BOOL,       "CFG_BOOL",       false};
    FlashParamNs::Parameter<uint8_t>     P_CFG_UINT8     {CFG_UINT8,      "CFG_UINT8",      23};
    FlashParamNs::Parameter<uint16_t>    P_CFG_UINT16    {CFG_UINT16,     "CFG_UINT16",     4096};
    FlashParamNs::Parameter<uint32_t>    P_CFG_UINT32    {CFG_UINT32,     "CFG_UINT32",     65535*4};
    FlashParamNs::Parameter<uint64_t>    P_CFG_UINT64    {CFG_UINT64,     "CFG_UINT64",     1ULL<<40};
    FlashParamNs::Parameter<int8_t>      P_CFG_INT8      {CFG_INT8,       "CFG_INT8",       -16};
    FlashParamNs::Parameter<int16_t>     P_CFG_INT16     {CFG_INT16,      "CFG_INT16",      -2047};
    FlashParamNs::Parameter<int32_t>     P_CFG_INT32     {CFG_INT32,      "CFG_INT32",      -65536*5};
    FlashParamNs::Parameter<int64_t>     P_CFG_INT64     {CFG_INT64,      "CFG_INT64",      -(1LL<<35)};
    FlashParamNs::Parameter<float>       P_CFG_FLOAT     {CFG_FLOAT,      "CFG_FLOAT",      3.326f};
    FlashParamNs::Parameter<double>      P_CFG_DOUBLE    {CFG_DOUBLE,     "CFG_DOUBLE",     -1.056e-8};
};
