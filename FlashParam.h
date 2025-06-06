/*-----------------------------------------------------------/
/ FlashParam.h
/------------------------------------------------------------/
/ Copyright (c) 2024, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/-----------------------------------------------------------*/

#pragma once

#include <map>
#include <string>
#include <cinttypes>  // this must be located at later than <string>
#include <variant>

#include "UserFlash.h"

namespace FlashParamNs {
//=================================
// Interface of Parameter class
//=================================
template <class T>
class Parameter {
    using valueType = T;
public:
    Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue, const size_t& size);
    Parameter(const uint32_t& id, const char* name, const uint32_t& flashAddr, const valueType& defaultValue) : Parameter(id, name, flashAddr, defaultValue, sizeof(T)) {};
    Parameter(const uint32_t& id, const char* name, const valueType& defaultValue, const size_t& size);
    Parameter(const uint32_t& id, const char* name, const valueType& defaultValue) : Parameter(id, name, defaultValue, sizeof(T)) {};
    void set(const valueType& value_) { value = value_; }
    const valueType& get() const { return value; }
    void loadDefault() { value = defaultValue; }
    const valueType& getDefault() const { return defaultValue; }
    const valueType& getFromFlash();
private:
    Parameter(const Parameter&) = delete;
    Parameter& operator=(const Parameter&) = delete;  // don't permit copy
    const uint32_t id;
    const char* name;
    const uint32_t flashAddr;
    const valueType defaultValue;
    const size_t size;
    valueType value = defaultValue;
    friend class Params;
    friend class FlashParam;
    friend class ReadFromFlashVisitor;
    friend class WriteReserveVisitor;
    friend class PrintInfoVisitor;
};

using variant_t = std::variant<
    Parameter<bool>*,
    Parameter<uint8_t>*, Parameter<uint16_t>*, Parameter<uint32_t>*, Parameter<uint64_t>*,
    Parameter<int8_t>*, Parameter<int16_t>*, Parameter<int32_t>*, Parameter<int64_t>*,
    Parameter<float>*, Parameter<double>*, Parameter<std::string>*
    >;

//=================================
// Interface of Visitors
//=================================
struct FlashVisitor {
    UserFlash& userFlash = UserFlash::instance();
};

struct ReadFromFlashVisitor : FlashVisitor {
    template <typename T>
    void operator()(const T& param) const {
        userFlash.read(param->flashAddr, param->size, param->value);
    }
};

struct WriteReserveVisitor : FlashVisitor {
    template <typename T>
    void operator()(const T& param) const {
        userFlash.writeReserve(param->flashAddr, param->size, param->value);
    }
};

struct PrintInfoVisitor {
    void operator()(const Parameter<bool>* param) const { printf("0x%04x %s: %s\n", param->flashAddr, param->name, param->value ? "true" : "false"); }
    void operator()(const Parameter<uint8_t>* param) const { printf("0x%04x %s: %" PRIu8 "d (0x%" PRIx8 ")\n", param->flashAddr, param->name, param->value, param->value); }
    void operator()(const Parameter<uint16_t>* param) const { printf("0x%04x %s: %" PRIu16 "d (0x%" PRIx16 ")\n", param->flashAddr, param->name, param->value, param->value); }
    void operator()(const Parameter<uint32_t>* param) const { printf("0x%04x %s: %" PRIu32 "d (0x%" PRIx32 ")\n", param->flashAddr, param->name, param->value, param->value); }
    void operator()(const Parameter<uint64_t>* param) const { printf("0x%04x %s: %" PRIu64 "d (0x%" PRIx64 ")\n", param->flashAddr, param->name, param->value, param->value); }
    void operator()(const Parameter<int8_t>* param) const { printf("0x%04x %s: %" PRIi8 "d (0x%" PRIx8 ")\n", param->flashAddr, param->name, param->value, param->value); }
    void operator()(const Parameter<int16_t>* param) const { printf("0x%04x %s: %" PRIi16 "d (0x%" PRIx16 ")\n", param->flashAddr, param->name, param->value, param->value); }
    void operator()(const Parameter<int32_t>* param) const { printf("0x%04x %s: %" PRIi32 "d (0x%" PRIx32 ")\n", param->flashAddr, param->name, param->value, param->value); }
    void operator()(const Parameter<int64_t>* param) const { printf("0x%04x %s: %" PRIi64 "d (0x%" PRIx64 ")\n", param->flashAddr, param->name, param->value, param->value); }
    void operator()(const Parameter<float>* param) const { printf("0x%04x %s: %7.4f (%7.4e)\n", param->flashAddr, param->name, param->value, param->value); }
    void operator()(const Parameter<double>* param) const { printf("0x%04x %s: %7.4f (%7.4e)\n", param->flashAddr, param->name, param->value, param->value); }
    void operator()(const Parameter<std::string>* param) const { printf("0x%04x %s: %s\n", param->flashAddr, param->name, param->value.c_str()); }
};

//=================================
// Interface of Params class
//=================================
class Params
{
// all private except for friend classes
    static constexpr uint32_t PRIME0 = 0x61e77795;
    static constexpr uint32_t PRIME1 = 0x8089f3a3;
    static constexpr uint32_t PRIME2 = 0xcdae6891;
    static Params& instance(); // Singleton
    void printInfo() const;
    void loadDefault();
    void loadFromFlash();
    bool storeToFlash() const;
    template <typename T>
    void add(const uint32_t& id, T* param) {
        paramMap[id] = param;
        // update mapHash
        const variant_t item = param;
        mapHash += param->flashAddr*PRIME0 + param->size*PRIME1 + item.index()*PRIME2;
    }
    template <typename T>
    T& getParam(const uint32_t& id) {
        auto& item = paramMap.at(id);
        auto& paramPtr = std::get<T*>(item);
        return *paramPtr;
    }
    uint32_t getNextFlashAddr() const { return nextFlashAddr; }
    void setNextFlashAddr(uint32_t addr) { nextFlashAddr = addr; }
    uint32_t getMapHash() const { return mapHash; }
    std::map<const uint32_t, variant_t> paramMap;
    uint32_t nextFlashAddr = 0;
    uint32_t mapHash = 0;
    template<typename> friend class Parameter;  // for all Parameter<> classes
    friend class FlashParam;
};

//=================================
// Interface of FlashParam class
//=================================
typedef enum {
    CFG_MAP_HASH = 0,
    CFG_STORE_COUNT,
    CFG_ID_BASE
} ParamIdBase_t;

class FlashParam
{
public:
    virtual void initialize(bool preserveStoreCount = false);
    virtual bool finalize();
    virtual void loadDefault(bool preserveStoreCount = false);
    virtual void printInfo() const;
    // accessor by id on template T = primitive type
    template <typename T>
    decltype(auto) getValue(const uint32_t& id) const { return _getValue<Parameter<T>>(id); }
    template <typename T>
    void setValue(const uint32_t& id, const T& value) { _setValue<Parameter<T>>(id, value); }

protected:
    FlashParam() = default;
    ~FlashParam() = default;
    FlashParam(const FlashParam&) = delete;
    FlashParam& operator=(const FlashParam&) = delete;
    // accessor by uint32_t on template T = Patameter<>
    template <typename T>
    void _setValue(const uint32_t& id, const typename T::valueType& value) {
        auto& param = Params::instance().getParam<T>(id);
        return param.set(value);
    }
    template <typename T>
    decltype(auto) _getValue(const uint32_t& id) const {
        const auto& param = Params::instance().getParam<T>(id);
        return param.get();
    }

    // built-in parameters
    // Parameter<T>        instance          id                name              default
    Parameter<uint32_t>    P_CFG_MAP_HASH   {CFG_MAP_HASH,    "CFG_MAP_HASH",    0};
    Parameter<uint32_t>    P_CFG_STORE_COUNT{CFG_STORE_COUNT, "CFG_STORE_COUNT", 0};
};
}
