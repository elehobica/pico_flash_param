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
    : Parameter(id, name, Params::instance().getNextFlashAddr(), defaultValue, size)
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
    printf("=== ConfigParam ===\n");
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
void FlashParam::printInfo() const
{
    UserFlash& userFlash = UserFlash::instance();
    userFlash.printInfo();
    Params::instance().printInfo();
}

void FlashParam::loadDefault()
{
    Params::instance().loadDefault();
}

void FlashParam::loadFromFlash()
{
    Params::instance().loadFromFlash();
}

bool FlashParam::storeToFlash() const
{
    return Params::instance().storeToFlash();
}
}
