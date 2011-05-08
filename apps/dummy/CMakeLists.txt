CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT (DUMMY)

INCLUDE_DIRECTORIES (../../libASE/include)

SET (CMAKE_C_FLAGS "-Wall -DDUMMY -mtune=core2 -O3 -std=c99")

ADD_SUBDIRECTORY (./controllers/timer ./bin/dummy-timer EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY (./controllers/SPSA ./bin/dummy-SPSA EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY (./controllers/ANN-test ./bin/dummy-ANN-test EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY (./controllers/GA-test ./bin/dummy-GA-test EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY (./controllers/GANN-test ./bin/dummy-GANN-test EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY (./controllers/SPSANN-test ./bin/dummy-SPSANN-test EXCLUDE_FROM_ALL)

