INCLUDE (${ASE_SOURCE_DIR}/src/CMakeLists.txt)

SET (SRCS
        AtronTosApi.c
        AbstractAtronTosApi.c
        ${CONTROL_DIR}/behaviors/atron/self-reconfiguration/SnakeToQuadruped_8M/SnakeToQuadruped_8M.c
        ${CONTROL_DIR}/behaviors/atron/self-reconfiguration/QuadrupedToSnake_8M/QuadrupedToSnake_8M.c
        ${CONTROL_DIR}/behaviors/atron/locomotion/QuadrupedGait_8M/QuadrupedGait_8M.c
        ${CONTROL_DIR}/behaviors/atron/locomotion/SnakeGait/SnakeGait.c
        ${TOOLS_DIR}/atron/AtronStatus/AtronStatus.c
)
SET (CMAKE_C_COMPILER "avr-gcc")
SET (CMAKE_AR "avr-ar")
SET (CMAKE_C_FLAGS "-mmcu=atmega128 -c -Wall -DTINYOS -DATRON")

ADD_LIBRARY (atron-tos STATIC ${SRCS} ${FULL_SRCS})
SET_TARGET_PROPERTIES (atron-tos PROPERTIES OUTPUT_NAME "ASE")
