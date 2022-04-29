cmake_minimum_required (VERSION 3.8)
project ("raptorspectrum")

#############################
# Create base
#############################

find_package (FFTW3f CONFIG REQUIRED)

set(raptorspectrum-sources "src/fft/fft.h" "src/fft/fft.cpp" "src/fft/fft_dispatcher_threaded.h" "src/fft/fft_dispatcher_threaded.cpp" "include/raptorspectrum.h" "src/fft/fft_dispatcher_base.h" "src/fft/fft_dispatcher_realtime.h" "src/fft/fft_dispatcher_realtime.cpp" "src/fft/fft_dispatcher_base.cpp" "src/client/iclient.h" "src/client/client_impl.h" "src/client/client_impl.cpp"   "src/client/post_process.h" "src/client/post_process.cpp" "src/server/iserver.h" "src/server/server_impl.h" "src/server/server_impl.cpp" "src/server/api.cpp" "src/client/api.cpp")

#############################
# Create shared library
#############################

add_library (raptorspectrum SHARED ${raptorspectrum-sources})
target_include_directories(raptorspectrum PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(raptorspectrum PUBLIC FFTW3::fftw3f)

#############################
# Create static library
#############################

add_library (raptorspectrum-static STATIC ${raptorspectrum-sources})
target_include_directories(raptorspectrum-static PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(raptorspectrum-static PUBLIC FFTW3::fftw3f)

#############################
# Create examples
#############################

option(RAPTORSPECTRUM_BUILD_EXAMPLE "Build examples" OFF)
if (RAPTORSPECTRUM_BUILD_EXAMPLE)
add_subdirectory(example)
endif ()

#############################
# Create installation target
#############################

install(TARGETS raptorspectrum EXPORT raptorspectrum-targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
  DESTINATION
    include
  COMPONENT
    Devel
)

export(PACKAGE raptorspectrum)
configure_file(cmake/raptorspectrum-config.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/raptorspectrum/raptorspectrum-config.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/raptorspectrum)
install(EXPORT raptorspectrum-targets
  FILE
    raptorspectrum-targets.cmake
  DESTINATION
    ${ConfigPackageLocation}
)

install(
  FILES
    cmake/raptorspectrum-config.cmake
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
