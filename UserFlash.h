/*------------------------------------------------------/
/ Copyright (c) 2021, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/------------------------------------------------------*/

#pragma once

#include <array>
#include <string>

#include "hardware/flash.h"

namespace FlashParamNs {
//=================================
// Interface of UserFlash class
//=================================
class UserFlash
{
public:
    static UserFlash& instance(); // Singleton
    void printInfo();
    template <typename T>
    void read(const uint32_t& flash_ofs, const size_t& size, T& value) {
        if (flash_ofs + size <= PageProgSize) {
            auto ptr = reinterpret_cast<uint8_t*>(&value);
            std::copy(flashContents + flash_ofs, flashContents + flash_ofs + size, ptr);
        }
    }
    void read(const uint32_t& flash_ofs, const size_t& size, std::string& value) {
        if (flash_ofs + size <= PageProgSize) {
            value.clear();
            std::copy(flashContents + flash_ofs, flashContents + flash_ofs + size, std::back_inserter(value));
        }
    }
    template <typename T>
    void writeReserve(const uint32_t& flash_ofs, const size_t& size, const T& value) {
        if (flash_ofs + size <= PageProgSize) {
            auto ptr = reinterpret_cast<const uint8_t*>(&value);
            std::copy(&ptr[0], &ptr[0] + size, data.data() + flash_ofs);
        }
    }
    void writeReserve(const uint32_t& flash_ofs, const size_t& size, const std::string& value) {
        if (flash_ofs + size <= PageProgSize) {
            std::copy(&value[0], &value[0] + size, data.data() + flash_ofs);
        }
    }
    bool program();
    bool clear();
    void dump();

protected:
    // PICO_FLASH_SIZE_BYTES: from pico-sdk/src/boards/include/boards/*.h
    // FLASH_xxx_SIZE       : from pico-sdk/src/rp2_common/hardware_flash/include/hardware/flash.h
    static constexpr size_t UserReqSize = 1024; // Byte
    static constexpr size_t EraseSize = ((UserReqSize + (FLASH_SECTOR_SIZE - 1)) / FLASH_SECTOR_SIZE) * FLASH_SECTOR_SIZE;
    static constexpr size_t PageProgSize = ((UserReqSize + (FLASH_PAGE_SIZE - 1)) / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
    static constexpr uint32_t UserFlashOfs = PICO_FLASH_SIZE_BYTES - EraseSize;
    UserFlash();
    virtual ~UserFlash();
    UserFlash(const UserFlash&) = delete;
    UserFlash& operator=(const UserFlash&) = delete;
    void _programCore();
    void _printValue(const char* name, int value, bool decimal = false);
    const uint8_t* flashContents = reinterpret_cast<const uint8_t*>(XIP_BASE + UserFlashOfs);
    std::array<uint8_t, PageProgSize> data;

    friend void _user_flash_program_core(void*);
    friend class FlashParam;
};
}
