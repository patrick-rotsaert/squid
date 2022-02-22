//
// Copyright (C) 2022 Patrick Rotsaert
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

// clang-format off

#if defined _WIN32 || defined __CYGWIN__
  #define SQUID_API_IMPORT __declspec(dllimport)
  #define SQUID_API_EXPORT __declspec(dllexport)
  #define SQUID_API_LOCAL
#else
  #if __GNUC__ >= 4
    #define SQUID_API_IMPORT __attribute__ ((visibility ("default")))
    #define SQUID_API_EXPORT __attribute__ ((visibility ("default")))
    #define SQUID_API_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define SQUID_API_IMPORT
    #define SQUID_API_EXPORT
    #define SQUID_API_LOCAL
  #endif
#endif

#ifdef SQUID_SHARED // compiled as a shared library
  #ifdef SQUID_SHARED_EXPORTS // defined if we are building the SQUID DLL (instead of using it)
    #define SQUID_API SQUID_API_EXPORT
  #else
    #define SQUID_API SQUID_API_IMPORT
  #endif // SQUID_SHARED_EXPORTS
  #define SQUID_LOCAL SQUID_API_LOCAL
#else // compiled as a static library
  #define SQUID_API
  #define SQUID_LOCAL
#endif // SQUID_SHARED
