# finjin-common
Finjin common is a library used by multiple Finjin projects.

## Highlights
* Multithreaded job system.
* Generic file system, zip, and file reading support.
* Collection, string, path, URI, UUID, math, date, and time duration classes.
* File format support for fstd, fsbd, json, cfg and xml files.
* Custom memory management classes.
* Generic class type system.

## Repository Structure
```
LICENSE.txt                 License file.
README.md                   This file.
configure.cfg               Source code configuration file. Modify this by hand when you start development for the very first time.
configure.py                Configuration script. Run this once  when you start development for the very first time.
cpp/library/                Main source tree.
  include/finjin/common/    Public include files.
  project/                  IDE project files for targeting different platforms.
  src/                      Implementation files and private include files.
tools/                      Various utility scripts.
wxcpp/library/src           Include and implementation files for use with wxWidgets.
```
