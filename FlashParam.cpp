/*-----------------------------------------------------------/
/ FlashParam.h
/------------------------------------------------------------/
/ Copyright (c) 2024, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/-----------------------------------------------------------*/

#include "FlashParam.h"

namespace FlashParamNs {

//=================================
// Implementation of Parameter class
//=================================
template <class T>
Parameter<T>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size)
    : id(id), name(name), flashAddr(flashAddr), defaultValue(defaultValue), size(size)
{
    Params& params = Params::instance();
    params.add(id, this);
    params.setNextFlashAddr(flashAddr + size);
}
template Parameter<bool>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<uint8_t>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<uint16_t>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<uint32_t>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<uint64_t>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<int8_t>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<int16_t>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<int32_t>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<int64_t>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<float>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<double>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<const char*>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
template Parameter<std::string>::Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);

template <class T>
Parameter<T>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size)
    : Parameter<T>(id, name, Params::instance().getNextFlashAddr(), defaultValue, size)
{
}
template Parameter<bool>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<uint8_t>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<uint16_t>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<uint32_t>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<uint64_t>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<int8_t>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<int16_t>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<int32_t>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<int64_t>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<float>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<double>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<const char*>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
template Parameter<std::string>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);

//=================================
// Implementation of Params class
//=================================
Params& Params::instance()
{
    static Params instance; // Singleton
    return instance;
}

void Params::printInfo() const
{
    printf("=== FlashParam ===\n");
    for (const auto& [key, item] : paramMap) {
        std::visit(PrintInfoVisitor{}, item);
    }
}

void Params::loadDefault()
{
    for (auto& [key, item] : paramMap) {
        std::visit([](auto&& param) {
            param->loadDefault();
        }, item);
    }
}

void Params::loadFromFlash()
{
    for (auto& [key, item] : paramMap) {
        std::visit(ReadFromFlashVisitor{}, item);
    }
}

bool Params::storeToFlash() const
{
    for (const auto& [key, item] : Params::instance().paramMap) {
        std::visit(WriteReserveVisitor{}, item);
    }
    UserFlash& userFlash = UserFlash::instance();
    return userFlash.program();
}

//=================================
// Implementation of FlashParam class
//=================================
void FlashParam::initialize()
{
    auto& params = Params::instance();
    params.loadDefault();

    // don't load from Flash if flash is blank or total size is different (format has changed?)
    if (getValueFromFlash(P_CFG_BOOT_COUNT) == 0xffffffffUL || getValueFromFlash(P_CFG_TOTAL_SUM) != params.getTotalSum()) {
        params.loadDefault();
        return;
    }

    params.loadFromFlash();
    incBootCount();
}

bool FlashParam::finalize()
{
    auto& params = Params::instance();
    P_CFG_TOTAL_SUM.set(params.getTotalSum());
    return Params::instance().storeToFlash();
}

void FlashParam::loadDefault()
{
    auto& params = Params::instance();
    params.loadDefault();
}

void FlashParam::incBootCount()
{
    auto& param = P_CFG_BOOT_COUNT;
    param.set(param.get() + 1);
}

void FlashParam::printInfo() const
{
    auto& userFlash = UserFlash::instance();
    userFlash.printInfo();
    auto& params = Params::instance();
    params.printInfo();
}
}
