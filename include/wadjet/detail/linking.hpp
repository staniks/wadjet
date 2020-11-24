#pragma once

#ifndef WADJET_STATIC
#ifdef WIN32
    #ifdef WADJET_EXPORTS
    #define WADJET_DLL __declspec(dllexport)
    #else
    #define WADJET_DLL __declspec(dllimport)
    #endif
#else
    #ifdef WADJET_EXPORTS
    #define WADJET_DLL __attribute__((visibility("default")))
    #else
    #define WADJET_DLL
    #endif
#endif
#else
    #define WADJET_DLL
#endif