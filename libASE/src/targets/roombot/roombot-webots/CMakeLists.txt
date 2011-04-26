#NEEDS TO INCLUDE WEBOTS C CONTROLLER CODe

INCLUDE (${ASE_SOURCE_DIR}/src/CMakeLists.txt)

SET (SRCS
        AbstractRoombotApi.c
        RoombotApi.c
        ${UTILS_DIR}/socket-tools/SimpleSocket.c
        ${UTILS_DIR}/socket-tools/CommandSender.c
        ${UTILS_DIR}/socket-tools/EventListner.c
)
SET (CMAKE_C_COMPILER "gcc")
SET (CMAKE_AR "ar")
SET (CMAKE_C_FLAGS "-c -Wall -DROOMBOT -DWEBOTS")

ADD_LIBRARY (roombot-webots STATIC ${SRCS} ${FULL_SRCS})

SET_TARGET_PROPERTIES (roombot-webots PROPERTIES OUTPUT_NAME "ASE")
