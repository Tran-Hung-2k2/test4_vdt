# Install script for directory: G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/smart_config")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "G:/esp/Espressif/tools/xtensa-esp32-elf/esp-2022r1-11.2.0/xtensa-esp32-elf/bin/xtensa-esp32-elf-objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/aes.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/aria.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/asn1.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/asn1write.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/base64.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/bignum.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/build_info.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/camellia.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ccm.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/chacha20.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/chachapoly.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/check_config.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/cipher.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/cmac.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/compat-2.x.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/config_psa.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/constant_time.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ctr_drbg.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/debug.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/des.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/dhm.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ecdh.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ecdsa.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ecjpake.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ecp.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/entropy.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/error.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/gcm.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/hkdf.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/hmac_drbg.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/legacy_or_psa.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/lms.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/mbedtls_config.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/md.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/md5.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/memory_buffer_alloc.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/net_sockets.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/nist_kw.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/oid.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/pem.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/pk.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/pkcs12.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/pkcs5.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/pkcs7.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/platform.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/platform_time.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/platform_util.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/poly1305.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/private_access.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/psa_util.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ripemd160.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/rsa.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/sha1.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/sha256.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/sha512.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ssl.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ssl_cache.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ssl_ciphersuites.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ssl_cookie.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/ssl_ticket.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/threading.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/timing.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/version.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/x509.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/x509_crl.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/x509_crt.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/mbedtls/x509_csr.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_builtin_composites.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_builtin_primitives.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_compat.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_config.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_driver_common.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_composites.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_primitives.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_extra.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_platform.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_se_driver.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_sizes.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_struct.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_types.h"
    "G:/esp/Espressif/frameworks/esp-idf-v5.0.2/components/mbedtls/mbedtls/include/psa/crypto_values.h"
    )
endif()

