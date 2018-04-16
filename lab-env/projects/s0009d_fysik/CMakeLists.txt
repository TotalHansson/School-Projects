#--------------------------------------------------------------------------
# S0009D Fysik
#--------------------------------------------------------------------------

PROJECT(s0009d_fysik)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)

SET(files_example ${example_headers} ${example_sources})
SOURCE_GROUP("example" FILES ${files_example})

ADD_EXECUTABLE(s0009d_fysik ${files_example})
TARGET_LINK_LIBRARIES(s0009d_fysik core render)
ADD_DEPENDENCIES(s0009d_fysik core render)
