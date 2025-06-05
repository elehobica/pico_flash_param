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
template Parameter<std::string>::Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);

template <class T>
const typename Parameter<T>::valueType& Parameter<T>::getFromFlash()
{
    FlashParamNs::ReadFromFlashVisitor visitor;
    visitor(this);
    return value;
}
template const typename Parameter<bool>::valueType& Parameter<bool>::getFromFlash();
template const typename Parameter<uint8_t>::valueType& Parameter<uint8_t>::getFromFlash();
template const typename Parameter<uint16_t>::valueType& Parameter<uint16_t>::getFromFlash();
template const typename Parameter<uint32_t>::valueType& Parameter<uint32_t>::getFromFlash();
template const typename Parameter<uint64_t>::valueType& Parameter<uint64_t>::getFromFlash();
template const typename Parameter<int8_t>::valueType& Parameter<int8_t>::getFromFlash();
template const typename Parameter<int16_t>::valueType& Parameter<int16_t>::getFromFlash();
template const typename Parameter<int32_t>::valueType& Parameter<int32_t>::getFromFlash();
template const typename Parameter<int64_t>::valueType& Parameter<int64_t>::getFromFlash();
template const typename Parameter<float>::valueType& Parameter<float>::getFromFlash();
template const typename Parameter<double>::valueType& Parameter<double>::getFromFlash();
template const typename Parameter<std::string>::valueType& Parameter<std::string>::getFromFlash();

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
    for (const auto& [key, item] : paramMap) {
        std::visit([](auto&& param) {
            param->loadDefault();
        }, item);
    }
}

void Params::loadFromFlash()
{
    for (const auto& [key, item] : paramMap) {
        std::visit(ReadFromFlashVisitor{}, item);
    }
}

bool Params::storeToFlash() const
{
    for (const auto& [key, item] : paramMap) {
        std::visit(WriteReserveVisitor{}, item);
    }
    UserFlash& userFlash = UserFlash::instance();
    return userFlash.program();
}

//=================================
// Implementation of FlashParam class
//=================================
void FlashParam::initialize(bool preserveStoreCount)
{
    loadDefault();

    // don't load from Flash if flash is blank
    if (P_CFG_STORE_COUNT.getFromFlash() == 0xffffffffUL) {
        return;
    }

    auto& params = Params::instance();
    // don't load from Flash if hash value is different (parhaps format has changed)
    if (P_CFG_MAP_HASH.getFromFlash() != params.getMapHash()) {
        loadDefault(preserveStoreCount);
        return;
    }

    // otherwise, load from Flash
    params.loadFromFlash();
}

bool FlashParam::finalize()
{
    auto& params = Params::instance();
    P_CFG_MAP_HASH.set(params.getMapHash());
    P_CFG_STORE_COUNT.set(P_CFG_STORE_COUNT.get() + 1);
    return params.storeToFlash();
}

void FlashParam::loadDefault(bool preserveStoreCount)
{
    auto& params = Params::instance();
    auto storeCount = P_CFG_STORE_COUNT.get();
    params.loadDefault();
    if (preserveStoreCount) { P_CFG_STORE_COUNT.set(storeCount); }
}

void FlashParam::printInfo() const
{
    auto& userFlash = UserFlash::instance();
    userFlash.printInfo();
    auto& params = Params::instance();
    params.printInfo();
}
}
