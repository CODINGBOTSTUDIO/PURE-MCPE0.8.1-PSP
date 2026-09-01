#include "platform/path.h"
#include "platform/savedata.h"

#include <pspiofilemgr.h>

#include <cstdio>
#include <cstring>

static char g_base[256] = "ms0:/PSP/GAME/MCPSP/";

void pathInit(const char* argv0) {
    if (!argv0 || !argv0[0])
        return;
    strncpy(g_base, argv0, sizeof(g_base) - 1);
    g_base[sizeof(g_base) - 1] = '\0';

    char* slash = strrchr(g_base, '/');
    if (slash)
        slash[1] = '\0';
}

const char* assetPath(const char* rel) {
    static char buf[320];
    snprintf(buf, sizeof(buf), "%s%s", g_base, rel);
    return buf;
}

static const char* const kSaveDir  = "Minecraft";
static const char* const kSaveRoot = "ms0:/PSP/SAVEDATA/Minecraft/";

const char* savePath(const char* rel) {
    static char buf[320];
    snprintf(buf, sizeof(buf), "%s%s", kSaveRoot, rel);
    return buf;
}

void savePathInit(void) {

    sceIoMkdir("ms0:/PSP", 0777);
    sceIoMkdir("ms0:/PSP/SAVEDATA", 0777);
    sceIoMkdir(kSaveRoot, 0777);
    sceIoMkdir(savePath("saves"), 0777);

    const char* sfoPath = savePath("PARAM.SFO");
    SceIoStat st;
    if (sceIoGetstat(sfoPath, &st) >= 0) return;

    static unsigned char sfo[8192];
    int n = sfoBuildSavedata(sfo, sizeof(sfo), "Minecraft Pocket-Edition",
                             "Worlds", "Saved worlds", kSaveDir);
    if (n <= 0) return;
    SceUID fd = sceIoOpen(sfoPath, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
    if (fd < 0) return;
    sceIoWrite(fd, sfo, n);
    sceIoClose(fd);
}
