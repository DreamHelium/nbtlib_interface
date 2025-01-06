cmake_minimum_required(VERSION 3.15)

project(nbt_interface LANGUAGES C)

find_package(PkgConfig)

pkg_search_module(GIO REQUIRED gio-2.0)

set(NI_SOURCE nbt_interface.c)

set(NI_HEADER nbt_interface.h)

set(NI_EXTERNAL_SOURCE libnbt/nbt.c libnbt/nbt.h)

add_library(dhnbt_interface SHARED ${NI_SOURCE} ${NI_HEADER} ${NI_EXTERNAL_SOURCE})

target_link_libraries(dhnbt_interface PUBLIC gio-2.0)
target_include_directories(dhnbt_interface PUBLIC ${GIO_INCLUDE_DIRS})