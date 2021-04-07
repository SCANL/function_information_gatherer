file(GLOB FUNCTION_DATA_SRC *.cpp)
file(GLOB FUNCTION_DATA_HPP *.hpp)

add_executable(gather_function_data ${FUNCTION_DATA_SRC} ${FUNCTION_DATA_HPP})

target_link_libraries( gather_function_data srcsaxeventdispatch srcsax_static ${LIBXML2_LIBRARIES} )