/*
    main.c - RB4DX
    Initialisation code for the RB4DX plugin.
    Licensed under the GNU Lesser General Public License version 2.1, or later.
*/

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define attr_module_hidden __attribute__((weak)) __attribute__((visibility("hidden")))
#define attr_public __attribute__((visibility("default")))

#define GOLDHEN_PATH "/data/GoldHEN"
#define RB4DX_PATH GOLDHEN_PATH "/AMP16DX"

#include <Common.h>
#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>
#include <orbis/Pad.h>
#include "plugin_common.h"

attr_public const char *g_pluginName = PLUGIN_NAME;
attr_public const char *g_pluginDesc = "Plugin for loading Amplitude (2016) Deluxe files.";
attr_public const char *g_pluginAuth = "LysiX, glitchgod";
attr_public uint32_t g_pluginVersion = 0x00000100; // 1.00

void DoNotificationStatic(const char* text) {
    OrbisNotificationRequest Buffer = { 0 };
    Buffer.useIconImageUri = 1;
    Buffer.targetId = -1;
    strcpy(Buffer.message, text);
    strcpy(Buffer.iconUri, "https://raw.githubusercontent.com/hmxmilohax/rock-band-3-deluxe/100069d1c2293424a659ecb4a5ddacc3b91c4f9b/dependencies/media/dx.png");
    sceKernelSendNotificationRequest(0, &Buffer, sizeof(Buffer), 0);
}

void DoNotification(const char *FMT, ...) {
    OrbisNotificationRequest Buffer = { 0 };
    va_list args;
    va_start(args, FMT);
    vsprintf(Buffer.message, FMT, args);
    va_end(args);
    Buffer.type = NotificationRequest;
    Buffer.unk3 = 0;
    Buffer.useIconImageUri = 1;
    Buffer.targetId = -1;
    strcpy(Buffer.iconUri, "https://raw.githubusercontent.com/hmxmilohax/rock-band-3-deluxe/100069d1c2293424a659ecb4a5ddacc3b91c4f9b/dependencies/media/dx.png");
    sceKernelSendNotificationRequest(0, &Buffer, sizeof(Buffer), 0);
}


bool USTitleID = true;

// ARKless file loading hook
const char* RawfilesFolder = "/data/GoldHEN/AMP16DX/";
const char* GameRawfilesFolder = "data:/GoldHEN/AMP16DX/";
bool PrintRawfiles = true;
bool PrintArkfiles = false;

typedef enum {
    kRead = 0x0,
    kReadNoArk = 0x1,
    kReadNoBuffer = 0x2,
    kAppend = 0x3,
    kWrite = 0x4,
    kWriteNoBuffer = 0x5
} FileMode;

void* (*NewFile)(const char*, FileMode);

HOOK_INIT(NewFile);

void NewFile_hook(const char* path, FileMode mode) {
    char rawpath[2048] = {0};
    strcat(rawpath, RawfilesFolder);
    /*if (rawpath[strlen(rawpath) - 1] != '/') {
        strcat(rawpath, "/");
    }*/
    strcat(rawpath, path);
    char gamepath[2048] = {0};
    strcat(gamepath, GameRawfilesFolder);
    strcat(gamepath, path);
    const char* newpath = rawpath;
    if (file_exists(newpath)) {
        if (PrintRawfiles) final_printf("Loading rawfile: %s\n", newpath);
        HOOK_CONTINUE(NewFile, void (*)(const char*, FileMode), gamepath, kReadNoArk);
        return;
    }
    if (PrintArkfiles || (PrintRawfiles && mode == kReadNoArk)) final_printf("Loading file: %s\n", path);
    HOOK_CONTINUE(NewFile, void (*)(const char*, FileMode), path, mode);
    return;
}


int32_t attr_public module_start(size_t argc, const void *args)
{
    if (sys_sdk_proc_info(&procInfo) != 0) {
        final_printf("shadPS4? assuming we're 01.01\n");
        // TODO: figure out version check and USTitleID check for shadPS4
    } else {
        final_printf("Started plugin! Title ID: %s\n", procInfo.titleid);
        if (strcmp(procInfo.titleid, "CUSA02480") == 0) {
            final_printf("US Amplitude Detected!\n");
            USTitleID = true;
        }
        else if (strcmp(procInfo.titleid, "CUSA02670") == 0) {
            final_printf("EU Amplitude Detected!\n");
            USTitleID = false;
        }
        else {
            final_printf("Game loaded is not Amplitude!\n");
            return 0;
        }
        
        if (strcmp(procInfo.version, "01.01") != 0) {
            final_printf("This plugin is only compatible with version 02.21 of Amplitude.\n");
            return 0;
        }
    }

    uint64_t base_address = get_base_address();
    
    final_printf("Applying Amp16DX hooks...\n");
    DoNotificationStatic("Amp16DX Plugin loaded!");

    NewFile = (void*)(base_address + 0x00253e30);
    
    // -- V1.0 Addresses
    //     NewFile = (void*)(procInfo.base_address + 0x00253530);
    //     SymbolSymbol = (void*)(procInfo.base_address + 0x005727c0); 


    // apply all hooks
    HOOK(NewFile);


    final_printf("\n                            dP oo   dP                  dP\n                            88      88                  88\n.d888b. 88d8b.d8b. 88d888b. 88 dP d8888P dP    dP .d888b88 .d8888b.\n88\' `88 88\'`88\'`88 88\'  `88 88 88   88   88    88 88\'  `88 88ooood8\n88. .88 88  88  88 88.  .88 88 88   88   88.  .88 88.  .88 88.  ...\n`888P\'8 dP  dP  dP 88Y888P\' dP dP   dP   `88888P\' `88888P8 `88888P\'\n                   88\n                   dP\n");

    return 0;
}

int32_t attr_public module_stop(size_t argc, const void *args)
{
    final_printf("Stopping plugin...\n");
    // unhook everything just in case
    UNHOOK(NewFile);
    return 0;
}
