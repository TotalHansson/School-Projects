#--------------------------------------------------------------------------
# half-edge_mesh_CCSubdivision
#--------------------------------------------------------------------------

PROJECT(half-edge_mesh_CCSubdivision)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)

SET(files_example ${example_headers} ${example_sources})
SOURCE_GROUP("half-edge_mesh_CCSubdivision" FILES ${files_example})

ADD_EXECUTABLE(half-edge_mesh_CCSubdivision ${files_example})
TARGET_LINK_LIBRARIES(half-edge_mesh_CCSubdivision core render)
ADD_DEPENDENCIES(half-edge_mesh_CCSubdivision core render)
