/*
 * Copyright (c) 2020 Andreas Pohl
 * Licensed under MIT (https://github.com/apohl79/audiogridder/blob/master/COPYING)
 *
 * Author: Andreas Pohl
 */

#include <JuceHeader.h>

#include "Utils.hpp"

#ifdef JUCE_WINDOWS
#include <windows.h>
#endif

namespace e47 {

String getLastErrorStr() {
#ifdef JUCE_WINDOWS
    DWORD err = GetLastError();
    LPSTR lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                  err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
    return String(lpMsgBuf);
#else
    std::vector<char> buf(512);
    ignoreUnused(strerror_r(errno, buf.data(), buf.size()));
    return String(buf.data());
#endif
}

bool isRunningUnderWine() {
#ifdef JUCE_WINDOWS
    static const bool underWine = [] {
        if (auto* ntdll = GetModuleHandleW(L"ntdll.dll")) {
            return GetProcAddress(ntdll, "wine_get_version") != nullptr;
        }
        return false;
    }();
    return underWine;
#else
    return false;
#endif
}

void windowToFront(Component* c) {
    setLogTagStatic("utils");
    traceScope();
    if (nullptr != c && isRunningUnderWine()) {
        // Raise the window among the Wine windows without activating the Wine app, otherwise the DAW loses focus
        c->toFront(false);
        return;
    }
    if (nullptr != c && !c->isAlwaysOnTop()) {
#ifndef JUCE_LINUX
        c->setAlwaysOnTop(true);
#endif
        c->toFront(true);
#ifndef JUCE_LINUX
        Timer::callAfterDelay(100, [c] { c->setAlwaysOnTop(false); });
#endif
    }
}

}  // namespace e47
