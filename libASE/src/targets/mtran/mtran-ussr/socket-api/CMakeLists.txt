INCLUDE (${ASE_SOURCE_DIR}/src/CMakeLists.txt)

SET (SRCS 
        MTRANSocketApi.c
        AbstractMTRANSocketApi.c
        ${UTILS_DIR}/socket-tools/SimpleSocket.c
        ${UTILS_DIR}/socket-tools/CommandSender.c
        ${UTILS_DIR}/socket-tools/EventListner.c
)
SET (CMAKE_C_COMPILER "gcc")
SET (CMAKE_AR "ar")
SET (CMAKE_C_FLAGS "-Wall -DMTRAN -DUSSR")


ADD_LIBRARY (mtran-ussr STATIC ${SRCS} ${FULL_SRCS})

#EventListner.c needs pthreads
TARGET_LINK_LIBRARIES (mtran-ussr STATIC phtread)

SET_TARGET_PROPERTIES (mtran-ussr PROPERTIES OUTPUT_NAME "ASE")
