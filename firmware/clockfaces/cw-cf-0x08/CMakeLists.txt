cmake_minimum_required(VERSION 3.5)

set(INCLUDE_DIRS)
set(SRC_DIRS)

if(CONFIG_CLOCKFACE_BME680)
  list(APPEND INCLUDE_DIRS ".")
  list(APPEND SRC_DIRS ".")
endif()

idf_component_register(
                       INCLUDE_DIRS ${INCLUDE_DIRS}
                       SRC_DIRS ${SRC_DIRS}
                       REQUIRES arduino Adafruit-GFX-Library Adafruit-BME680-Library cw-commons
                       )
project(cw-cf-0x08)
