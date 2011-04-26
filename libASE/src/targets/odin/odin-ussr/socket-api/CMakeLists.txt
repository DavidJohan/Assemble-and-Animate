INCLUDE (${ASE_SOURCE_DIR}/src/CMakeLists.txt)

SET (SRCS
        AbstractOdinApi.c
        OdinSocketApi.c
        ${UTILS_DIR}/socket-tools/SimpleSocket.c
        ${UTILS_DIR}/socket-tools/CommandSender.c
        ${UTILS_DIR}/socket-tools/EventListner.c
)

SET (CMAKE_C_FLAGS "-c -Wall -DODIN -DUSSR")
SET (CMAKE_C_COMPILER "gcc")
SET (CMAKE_AR "ar")

ADD_LIBRARY (odin-ussr STATIC ${SRCS} ${FULL_SRCS})
SET_TARGET_PROPERTIES (odin-ussr PROPERTIES OUTPUT_NAME "ASE")
