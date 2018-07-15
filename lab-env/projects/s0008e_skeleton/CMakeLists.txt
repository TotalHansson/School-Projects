#--------------------------------------------------------------------------
# S0008E Skeleton
#--------------------------------------------------------------------------

PROJECT(s0008e_skeleton)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cpp)

SET(files_example ${example_headers} ${example_sources})
SOURCE_GROUP("skeleton" FILES ${files_example})

ADD_EXECUTABLE(s0008e_skeleton ${files_example})
TARGET_LINK_LIBRARIES(s0008e_skeleton core render)
ADD_DEPENDENCIES(s0008e_skeleton core render)
