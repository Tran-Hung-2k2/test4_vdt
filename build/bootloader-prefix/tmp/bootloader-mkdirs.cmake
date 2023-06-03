# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/bootloader/subproject"
  "G:/esp/Esp32/bai2/build/bootloader"
  "G:/esp/Esp32/bai2/build/bootloader-prefix"
  "G:/esp/Esp32/bai2/build/bootloader-prefix/tmp"
  "G:/esp/Esp32/bai2/build/bootloader-prefix/src/bootloader-stamp"
  "G:/esp/Esp32/bai2/build/bootloader-prefix/src"
  "G:/esp/Esp32/bai2/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "G:/esp/Esp32/bai2/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "G:/esp/Esp32/bai2/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
