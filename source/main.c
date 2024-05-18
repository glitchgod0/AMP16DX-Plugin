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
#define PLUGIN_CONFIG_PATH RB4DX_PATH "/AMP16DX.ini"
#define PLUGIN_NAME "AMP16DX"
#define final_printf(a, args...) klog("[" PLUGIN_NAME "] " a, ##args)

#include <Common.h>
#include <orbis/libkernel.h>
#include <orbis/Sysmodule.h>
#include <orbis/Pad.h>
#include "ini.h"

attr_public const char *g_pluginName = PLUGIN_NAME;
attr_public const char *g_pluginDesc = "Plugin for loading Amplitude (2016) Deluxe files.";
attr_public const char *g_pluginAuth = "LysiX, glitchgod";
attr_public uint32_t g_pluginVersion = 0x00000100; // 1.00

void DoNotificationStatic(const char* text) {
    OrbisNotificationRequest Buffer = { 0 };
    Buffer.useIconImageUri = 1;
    Buffer.targetId = -1;
    strcpy(Buffer.message, text);
    strcpy(Buffer.iconUri, "cxml://psnotification/tex_icon_system");
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
    strcpy(Buffer.iconUri, "cxml://psnotification/tex_icon_system");
    sceKernelSendNotificationRequest(0, &Buffer, sizeof(Buffer), 0);
}

bool file_exists(const char* filename) {
    struct stat buff;
    return stat(filename, &buff) == 0 ? true : false;
}

static struct proc_info procInfo;

// ARKless file loading hook
const char* RawfilesFolder = "/data/GoldHEN/AMP16DX/";
const char* GameRawfilesFolder = "data:/GoldHEN/AMP16DX/";
bool USTitleID = true;
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
    char rawpath[256];
    strcpy(rawpath, RawfilesFolder);
    /*if (rawpath[strlen(rawpath) - 1] != '/') {
        strcat(rawpath, "/");
    }*/
    strcat(rawpath, path);
    char gamepath[256];
    strcpy(gamepath, GameRawfilesFolder);
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

#define ADDR_OFFSET 0x00400000
int32_t attr_public module_start(size_t argc, const void *args)
{
    if (sys_sdk_proc_info(&procInfo) != 0) {
        DoNotificationStatic("Failed to get process info!");
        return 0;
    }

    sys_sdk_proc_info(&procInfo);
    final_printf("Started plugin! Title ID: %s\n", procInfo.titleid);
    if (strcmp(procInfo.titleid, "CUSA02480") == 0) {
        DoNotificationStatic("US Amp16 Detected!");
        USTitleID = true;
    }
    else {
        DoNotificationStatic("Game loaded is not Amp16!");
        return 0;
    }
    
    if (strcmp(procInfo.version, "01.01") != 0) {
        DoNotificationStatic("This plugin is only compatible with version 01.01 of Amplitude.");
        return 0;
    }

    final_printf("Applying Amp16DX hooks...\n");
    DoNotificationStatic("Amp16DX Plugin loaded!");

    NewFile = (void*)(procInfo.base_address + 0x00253e30);

    // apply all hooks
    HOOK(NewFile);

    return 0;
}

int32_t attr_public module_stop(size_t argc, const void *args)
{
    final_printf("Stopping plugin...\n");
    // unhook everything just in case
    UNHOOK(NewFile);
    return 0;
}
