#include "WipeoutPatcher.hpp"

#include "Config.hpp"
#include "Logger.hpp"

namespace glrage {

void WipeoutPatcher::apply(Config& config)
{
    // Wipeout 95 apparently has a bug in the keyboard handling, which sometimes
    // produces random crashes when a key is pressed for a prolonged period of
    // time.
    // The devs decided it would be a good idea to tamper with the system
    // settings to reduce the key repeat rate to a minimum in order to "fix"
    // this bug. The screensaver is also disabled for some unrelated reason.
    // Unfortunately, these custom settings are not restored correctly when the
    // game crashes or otherwise exits unexpectedly, much to the annoyance of
    // the user.
    // This patch replaces the SystemParametersInfo function with a stub to
    // disable that behavior.
    m_tmp.clear();
    m_tmp << hookSystemParametersInfoA;

    patch(0x7E0290, m_tmp); // build 30.10.1996
    // TODO: doesn't work in older builds: video plays in background, but
    // window won't appear and main thread is stuck in PeekMessage loop
    //patch(0x7D0290, m_tmp); // build 22.07.1996 / 07.07.1996

    // Disable bugged alt+tab check that causes a segfault after playing the
    // into video on Windows 10 and possibly older versions as well.
    // GLRage doesn't really need it anyway and the only side effect is that the
    // game continues to run in the background when tabbed out.

    patchNop(0x46AFAD, "C7 05 8B DE 6A 00 00 00 00 00"); // build 30.10.1996
    patchNop(0x458785, "89 0D 7C E3 69 00");             // build 22.07.1996
    patchNop(0x458825, "89 2D 7C E3 69 00");             // build 07.07.1996

    // Disable unskippable title screen, saves a few seconds of wait time.
    if (config.getBool("disable_title_screen", false)) {
        patchNop(0x46B885, "E8 B8 40 00 00"); // build 30.10.1996
        patchNop(0x458FF3, "E8 F4 34 00 00"); // build 22.07.1996
        patchNop(0x459087, "E8 F4 34 00 00"); // build 07.07.1996
    }

    // Disable introductory video.
    if (config.getBool("disable_introductory_video", false)) {
        patchNop(0x46B808, "E8 33 46 00 00"); // build 30.10.1996
        patchNop(0x458F9A, "E8 C5 39 00 00"); // build 22.07.1996
        patchNop(0x45902E, "E8 C5 39 00 00"); // build 07.07.1996
    }
}

BOOL WipeoutPatcher::hookSystemParametersInfoA(
    UINT uiAction, UINT uiParam, PVOID pvParam, UINT fWinIni)
{
    // do nothing
    return TRUE;
}

} // namespace glrage