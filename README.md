ChibiOS-JS
==========

ChibiOS Javascript environment for STM32F4 discovery Kit

This project provides an javascript environment with a basic RTOS functionality
(courtesy of ChibiOS) At this moment it is only a basic implementation.

This is a try to emulate espurino javascript enviroment with a FLOSS tools.

Status
======

* Basic ChibiOS 2.6.0 enviroment with usb cdc terminal
* Command shell (no pipes, no process, no fg, no bg)
* Basic POSIX compativility (open/close/write/read only for ChibiOS streams)
* C++ runtime with exceptions (no RTTI) and std::containers support (thanks to libstdc++)
* TinyJS interpreter with advance line edition

TODO
====

* All of Espurino JS function compativility (Less or more similar to Arduino API)
* Network support (ethernet, ppp, slip? Sure, LwIP based)
* VFS support (FatFS, ROMFS, RAMFS)
* Real POSIX layer (getenv/setenv, socket/bind/listen/etc)
* Real serial-line-disipline (aka termcap) with line edition, echo/no-echo, CR/CR+LF
* curses support (PDCurses based? ncurses based? minux curses based? Original BSD curses?)
* Memory pool for Javascript support (Based on chMemoryPool? C++ templated based?)
* Support for more (and more, and more) plataform than F4Discovery:
  - Emtech Lanin CM4 v1
  - Olimex E/H407
  - Generic F4x5/7 boards

Delirant TODO (aka non promised TODO)
=====================================
* VGA output via DMA
* PS2 Key/Mice input via interrupt
* USB HOST support (any good framework?)
