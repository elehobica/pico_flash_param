/*-----------------------------------------------------------/
/ ConfigParam.h
/------------------------------------------------------------/
/ Copyright (c) 2024, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/-----------------------------------------------------------*/

#include "ConfigParam.h"

//=================================
// Implementation of ConfigParam class
//=================================
ConfigParam& ConfigParam::instance()  // Singleton
{
    static ConfigParam instance;
    return instance;
}

void ConfigParam::incBootCount()
{
    auto& param = P_CFG_BOOT_COUNT;
    param.set(param.get() + 1);
}

void ConfigParam::initialize()
{
    loadDefault();

    // don't load from Flash if flash is blank or total size is different (format has changed?)
    if (getValueFromFlash(P_CFG_BOOT_COUNT) == 0xffffffffUL || getValueFromFlash(P_CFG_TOTAL_SUM) != getTotalSum()) {
        loadDefault();
        return;
    }

    loadFromFlash();
    incBootCount();
}

bool ConfigParam::finalize()
{
    P_CFG_TOTAL_SUM.set(getTotalSum());
    return storeToFlash();
}