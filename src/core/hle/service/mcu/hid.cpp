// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include "core/hle/service/mcu/hid.h"

namespace Service {
namespace MCU {

HID::HID(std::shared_ptr<Module> mcu) : Module::Interface(std::move(mcu), "mcu::HID", 1) {
    static const FunctionInfo functions[] = {
        {0x00010040, nullptr, "unk_0x00010040"},
        {0x00020000, nullptr, "unk_0x00020000"},
        {0x00030040, nullptr, "unk_0x00030040"},
        {0x00040000, nullptr, "unk_0x00040000"},
        {0x00050080, nullptr, "unk_0x00050080"},
        {0x00060000, nullptr, "ReadGyroscopeValues"},
        {0x00070000, nullptr, "GetRaw3DSliderPosition"},
        {0x00080040, nullptr, "unk_0x00080040"},
        {0x00090000, nullptr, "unk_0x00090000"},
        {0x000A0040, nullptr, "unk_0x000A0040"},
        {0x000B0000, nullptr, "unk_0x000B0000"},
        {0x000C0000, nullptr, "GetMcuHidEventHandle"},
        {0x000D0000, nullptr, "GetMcuHidEventReason"},
        {0x000E0000, &HID::GetSoundVolume2, "GetSoundVolume"},
        {0x000F0040, nullptr, "EnableAccelerometerInterrupt"},
    };
    RegisterHandlers(functions);
}

} // namespace MCU
} // namespace Service
