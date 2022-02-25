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
3. ~~Add transaction class~~
4. Bind parameters by reference
5. Add logging and support custom logging backend
6. Add support for MySQL
7. Add packaging (CPack deb, rpm, ...?)
8. Build on Windows
9. Add support for ODBC
10. Add support for Oracle

## Quick start

TODO
