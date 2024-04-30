#pragma once

#ifndef LECS_DO_NOT_USE_DEFAULT_MACRO
    #if defined(__clang__)
        #define LECS_COMPILER_CLANG
    #elif defined(__GNUC__) || defined(__GNUG__)
        #define LECS_COMPILER_GCC
    #elif defined(_MSC_VER)
        #define LECS_COMPILER_MSVC
    #endif

    #ifdef LECS_COMPILER_MSVC
        #ifdef _DEBUG
            #define LECS_DEBUG
        #endif
    #endif

    #if defined(LECS_COMPILER_CLANG) || defined(LECS_COMPILER_GCC)
        #if !defined(NDEBUG)
            #define LECS_DEBUG
        #endif
    #endif
#endif
