INCLUDE (${ASE_SOURCE_DIR}/src/CMakeLists.txt)

SET (SRCS
        AbstractDynamixelApi.c
        DynamixelCM510Api.c
        firmware/dxl_hal.c
        firmware/dynamixel.c
        firmware/serial.c
        firmware/zgb_hal.c
        firmware/zigbee.c
)
SET (CMAKE_C_COMPILER "avr-gcc")
SET (CMAKE_AR "avr-ar")
SET (CMAKE_C_FLAGS "-mmcu=atmega2561 -c -Wall -DCM510 -DDYNAMIXEL")

ADD_LIBRARY (dynamixel-cm510 STATIC ${SRCS} ${FULL_SRCS})
SET_TARGET_PROPERTIES (dynamixel-cm510 PROPERTIES OUTPUT_NAME "ASE")
