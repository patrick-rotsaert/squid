# SQUID - Another C++ Database Access Library

## Introduction

This library was somewhat inspired by the [SOCI](https://github.com/SOCI/soci) library, 
which I have used for a long time. SOCI is a very powerful library, but the main
issue I have had with it is its exception safety. Running a "streamed" query 
or preparing a streamed statement in SOCI effectively happens in class destructors. 
Without very good care, this can cause the application to abort in case of exceptions.

However, I did like the SOCI architecture where the developer interacts mainly with a database-unaware 
frontend which passes on the work to a database-specific backend.
This idea I gladly kept but apart from that, this a totally different implementation.

For those wanting an acronym meaning, SQUID stands for **SQ**L: **U**nified **I**nterface to **D**atabases.

## License

The SQUID library is distributed under the terms of the [Boost Software License](http://www.boost.org/LICENSE_1_0.txt).
The file [InstallBasicPackageFiles.cmake](cmake/InstallBasicPackageFiles.cmake) is distributed under the terms of the [3-Clause BSD License](https://opensource.org/licenses/BSD-3-Clause).

## Requirements

### Common

* CMake >= 3.17
* C++20 compiler, build currently tested with:
   * Clang 13
   * GCC 11

### Database clients

Currently supported databases are:

* PostgreSQL
* SQLite3

Future planned:
* MySQL
* ODBC
* Oracle

## Short term TODO list

* Write quick start below

## Roadmap

1. ~~Add support for SQLite3~~
2. ~~Add connection pool~~
3. Add transaction class
4. Add logging and support custom logging backend
5. Add support for MySQL
6. Add packaging (CPack deb, rpm, ...?)
7. Build on Windows
8. Add support for ODBC
9. Add support for Oracle

## Quick start

TODO
