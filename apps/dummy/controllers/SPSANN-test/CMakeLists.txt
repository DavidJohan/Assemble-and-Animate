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
ADD_LIBRARY (controller-SPSANN-test ${LIB_SRCS})
TARGET_LINK_LIBRARIES (controller-SPSANN-test m)
ADD_EXECUTABLE (dummy-SPSANN-test ${SRCS})
TARGET_LINK_LIBRARIES (dummy-SPSANN-test controller-SPSANN-test ${ASE_LIB})
