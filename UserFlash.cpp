/*------------------------------------------------------/
/ Copyright (c) 2021, Elehobica
/ Released under the BSD-2-Clause
/ refer to https://opensource.org/licenses/BSD-2-Clause
/------------------------------------------------------*/

#include "UserFlash.h"

#include <cstdio>
#include <cstring>

#include "pico/flash.h"

namespace FlashParamNs {
void _user_flash_program_core(void* ptr)
{
    UserFlash* inst = static_cast<UserFlash*>(ptr);
    inst->_program_core();
}

//=================================
// Implementation of UserFlash class
//=================================
UserFlash& UserFlash::instance()
{
    static UserFlash instance; // Singleton
    return instance;
}

UserFlash::UserFlash()
{
    std::copy(flashContents, flashContents + data.size(), data.begin());
}

UserFlash::~UserFlash()
{
}

void UserFlash::printInfo()
{
    printf("=== UserFlash ===\n");
    printf("FlashSize: 0x%x (%d)\n", FlashSize, FlashSize);
    printf("SectorSize: 0x%x (%d)\n", FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
    printf("PageSize: 0x%x (%d)\n", FLASH_PAGE_SIZE, FLASH_PAGE_SIZE);
    printf("UserReqSize: 0x%x (%d)\n", UserReqSize, UserReqSize);
    printf("EraseSize: 0x%x (%d)\n", EraseSize, EraseSize);
    printf("PageProgSize: 0x%x (%d)\n", PageProgSize, PageProgSize);
    printf("UserFlashOfs: 0x%x (%d)\n", UserFlashOfs, UserFlashOfs);
}

bool UserFlash::program()
{
    // Need to stop interrupt during erase and program
    // noted that if core1 is running, it must be stopped also if accessing flash
    int result = flash_safe_execute(_user_flash_program_core, this, 100);
    if (result != PICO_OK) {
        return false;
    }
    return true;
}

bool UserFlash::clear()
{
    for (auto& item : data) { item = 0xff; }
    return program();
}

void UserFlash::dump()
{
    for (int i = 0; i < data.size(); i += 16) {
        for (int j = 0; j < 16; j++) {
            if (i + j >= data.size()) { break; }
            printf("%02x ", static_cast<int>(data.at(i+j)));
        }
        for (int j = 0; j < 16; j++) {
            if (i + j >= data.size()) { break; }
            if (data.at(i+j) >= 0x20 && data.at(i+j) <= 0x7E) {
                printf("%c", data.at(i+j));
            } else {
                printf(" ");
            }
        }
        printf("\r\n");
    }
}

void UserFlash::_program_core()
{
    flash_range_erase(UserFlashOfs, EraseSize);
    flash_range_program(UserFlashOfs, data.data(), data.size());
    std::copy(flashContents, flashContents + data.size(), data.begin());
}
}
