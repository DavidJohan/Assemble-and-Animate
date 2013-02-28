INCLUDE (${ASE_SOURCE_DIR}/src/CMakeLists.txt)

SET (SRCS
        AbstractFableApi.c
        FableApi.c
        ../dynamixel/DynamixelApi.c
        firmware/dxl_hal.c
        firmware/dynamixel.c
        firmware/serial.c
        firmware/accel.c
        firmware/ext_adc.c
        firmware/ext_uart.c
        firmware/gyro.c
        firmware/i2c.c
        firmware/ir.c
		firmware/ext_ee.c
		firmware/timer.c
		comm/datalink.c
		comm/frame.c
		comm/interfaces.c
        
        
)
SET (CMAKE_C_COMPILER "avr-gcc")
SET (CMAKE_AR "avr-ar")
SET (CMAKE_C_FLAGS "-mmcu=atmega2561 -Os -c -Wall -DFABLE -DDYNAMIXEL -DF_CPU=8000000UL -lc")

ADD_LIBRARY (fable STATIC ${SRCS} ${FULL_SRCS})
SET_TARGET_PROPERTIES (fable PROPERTIES OUTPUT_NAME "ASE")
