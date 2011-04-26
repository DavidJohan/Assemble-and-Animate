INCLUDE (${ASE_SOURCE_DIR}/src/CMakeLists.txt)

SET (SRCS
        AtronSocketApi.c
        AbstractAtronSocketApi.c
        ${CONTROL_DIR}/behaviors/atron/self-reconfiguration/SnakeToQuadruped_8M/SnakeToQuadruped_8M.c
        ${CONTROL_DIR}/behaviors/atron/self-reconfiguration/QuadrupedToSnake_8M/QuadrupedToSnake_8M.c
        ${CONTROL_DIR}/behaviors/atron/locomotion/QuadrupedGait_8M/QuadrupedGait_8M.c
        ${CONTROL_DIR}/behaviors/atron/locomotion/SnakeGait/SnakeGait.c
        ${TOOLS_DIR}/atron/AtronStatus/AtronStatus.c
        ${UTILS_DIR}/socket-tools/SimpleSocket.c
        ${UTILS_DIR}/socket-tools/CommandSender.c
        ${UTILS_DIR}/socket-tools/EventListner.c
)

SET (CMAKE_C_FLAGS "-c -Wall -DATRON -DUSSR")
SET (CMAKE_C_COMPILER "gcc")
SET (CMAKE_AR "ar")

ADD_LIBRARY (atron-ussr STATIC ${SRCS} ${FULL_SRCS})
SET_TARGET_PROPERTIES (atron-ussr PROPERTIES OUTPUT_NAME "ASE")
