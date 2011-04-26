SET (atron-ussr-ASE ../../../lib/atron-ussr/libASE.a)

SET (SRCS
        controller.c
        robot.c
)
ADD_EXECUTABLE (consensus ${SRCS})
TARGET_LINK_LIBRARIES (consensus ${atron-ussr-ASE})
TARGET_LINK_LIBRARIES (consensus pthread)

