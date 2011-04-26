INCLUDE (${ASE_SOURCE_DIR}/src/CMakeLists.txt)

SET (SRCS
        dxl_hal.c
        dynamixel.c
        AbstractDynamixelApi.c
        DynamixelD2UApi.c
        ${UTILS_DIR}/socket-tools/SimpleSocket.c
)
SET (CMAKE_C_COMPILER "gcc")
SET (CMAKE_AR "ar")
SET (CMAKE_C_FLAGS "-c -Wall -DDYNAMIXEL -DD2U")

ADD_LIBRARY (dynamixel-d2u STATIC ${SRCS} ${FULL_SRCS})

TARGET_LINK_LIBRARIES (dynamixel-d2u STATIC ${ASE_SOURCE_DIR}/extlib/ftd2xx/ftd2xx.lib)

SET_TARGET_PROPERTIES (dynamixel-d2u PROPERTIES OUTPUT_NAME "ASE")
