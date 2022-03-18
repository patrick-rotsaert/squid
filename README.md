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
* CMake >= 3.19
* Ninja, only required to build packages.

### Database clients

Currently supported databases are:

* PostgreSQL
* SQLite3

Future planned:
* MySQL
* ODBC
* Oracle

## TODO list

* Bind result -> enum
* Write unit tests
* Write quick start below

## Roadmap

1. ~~Add support for SQLite3~~
2. ~~Add connection pool~~
3. ~~Add transaction class~~
4. ~~Bind parameters by reference~~
5. Add logging and support custom logging backend
6. Add support for string encodings / charsets, std::wstring and wchar_t
7. Add support for MySQL
8. Add packaging (CPack ~~deb,~~ rpm, ...?)
9. Build on Windows
10. Add support for ODBC
11. Add support for Oracle

## Quick start

TODO when the API is stable.
