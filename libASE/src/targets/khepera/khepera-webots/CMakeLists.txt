INCLUDE (${ASE_SOURCE_DIR}/src/CMakeLists.txt)

SET (SRCS
        KheperaApi.c
        ${UTILS_DIR}/socket-tools/SimpleSocket.c
        ${UTILS_DIR}/socket-tools/CommandSender.c
        ${UTILS_DIR}/socket-tools/EventListner.c
)
SET (CMAKE_C_COMPILER "gcc")
SET (CMAKE_AR "ar")
SET (CMAKE_C_FLAGS "-c -Wall -DKHEPERA -DWEBOTS")

ADD_LIBRARY (khepera-webots STATIC ${SRCS} ${FULL_SRCS})

#EventListner.c needs pthreads
TARGET_LINK_LIBRARIES (khepera-webots STATIC phtread)

SET_TARGET_PROPERTIES (khepera-webots PROPERTIES OUTPUT_NAME "ASE")
