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
ADD_LIBRARY (controller-SPSA ${LIB_SRCS})
ADD_EXECUTABLE (dummy-SPSA ${SRCS})
TARGET_LINK_LIBRARIES (dummy-SPSA controller-SPSA ${ASE_LIB})
