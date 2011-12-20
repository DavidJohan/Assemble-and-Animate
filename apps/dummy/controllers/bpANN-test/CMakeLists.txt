INCLUDE_DIRECTORIES (${DUMMY_SOURCE_DIR}/dummy)
SET (LIB_SRCS
        controller.c
)
SET (SRCS
        ${DUMMY_SOURCE_DIR}/dummy/startDummy.c
)

FIND_LIBRARY (ASE_LIB NAME "ASE" PATHS ../../../../libASE/lib/dummy)
IF ("${ASE_LIB}" STREQUAL "ASE_LIB-NOT-FOUND")
        message(FATAL_ERROR "One of the libs wasn't found!")
ENDIF ()
ADD_LIBRARY (controller-bpANN-test ${LIB_SRCS})
TARGET_LINK_LIBRARIES (controller-bpANN-test m)
ADD_EXECUTABLE (dummy-bpANN-test ${SRCS})
TARGET_LINK_LIBRARIES (dummy-bpANN-test controller-bpANN-test ${ASE_LIB})
