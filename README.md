# SQUID - Another C++ Database Access Library

## Introduction

This library was somewhat inspired by the [SOCI](https://github.com/SOCI/soci) library, which I have used for a long time.
SOCI is a great and powerful library but some of its design choices, undoubtedly chosen to make life easier, make it hard to use at times:
* Streamed statements: The actual statement preparation or execution happens in class destructors.
  Without very good care, this can cause the application to abort in case of exceptions.
  In this library, destructors never throw.
* Query parameters are always bound by reference (soci::use).
  This can give unexpected results when passed temporaries.
  The SOCI documentation warns about this, but still it is easy to overlook and the compiler does not warn you.
  This library requires you to explicitly state if you bind a parameter by value or by reference.

However, I did like the SOCI architecture where the library user interacts mainly with a database-agnostic
frontend which passes on the work to a database-specific backend.
I gladly borrowed that idea but, apart from that, this a totally different implementation.

For those wondering... _SQUID_ is an acronym for **SQ**L: **U**nified **I**nterface to **D**atabases.

## License

The SQUID library is distributed under the terms of the [Boost Software License](http://www.boost.org/LICENSE_1_0.txt).

## Requirements

### Common

* C++20 compiler, build currently tested with:
   * Clang 11
   * Clang 13
   * GCC 11
   * MSVC 2019, currently only tested with SQLite.
* CMake >= 3.19
* Ninja, only required to build packages.

### Database clients

Currently supported databases are:

* PostgreSQL
* SQLite3
* MySQL

Future planned:
* ODBC
* Oracle

## TODO list

* Write unit tests (in progress)
* Write quick start below
* ~~Add conditional support for binding Boost.DateTime types~~
* Add conditional support for binding Howard Hinnant date types.
* ~~Add support for SQLite3~~
* ~~Add support for MySQL~~
* ~~Build on Windows~~
* Add support for ODBC
* Add support for Oracle
* ~~Add connection pool~~
* ~~Add transaction class~~
* ~~Bind parameters by reference~~
* Add logging and support custom logging backend
* Add support for string encodings / charsets, std::wstring and wchar_t
* Add packaging (CPack ~~deb,~~ rpm, ...?)

## Quick start

TODO when the API is stable.
