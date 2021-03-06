PROJECT(flow-features)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)





# Version number
SET(OF_VERSION_MAJOR 2)
SET(OF_VERSION_MINOR 0)

SET(SF_VERSION_MAJOR 2)
SET(SF_VERSION_MINOR 0)

SET(OF2_VERSION_MAJOR 2)
SET(OF2_VERSION_MINOR 0)

SET(SF2_VERSION_MAJOR 2)
SET(SF2_VERSION_MINOR 0)





# Build version
IF( NOT CMAKE_BUILD_TYPE )
    SET(CMAKE_BUILD_TYPE Release)
    MESSAGE("Release build.")
ELSEIF( CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-DDEBUG)
    MESSAGE("Debug build")
ENDIF()






# Set install variables
SET(OF_BINARY "opticalflowfeatures")
SET(SF_BINARY "sceneflowfeatures")
SET(OF2_BINARY "opticalflowfeatures2")
SET(SF2_BINARY "sceneflowfeatures2")

SET(OF_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${OF_BINARY})
SET(SF_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${SF_BINARY})
SET(OF2_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${OF2_BINARY})
SET(SF2_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${SF2_BINARY})

SET(PROJECT_BINARY_DIR ${PROJECT_BINARY_DIR}/build)

INCLUDE_DIRECTORIES(${OF_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SF_SOURCE_DIR})
INCLUDE_DIRECTORIES(${OF2_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SF2_SOURCE_DIR})





# Where are cmake modules
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake-modules)

# Where to put compiled files
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Where to install files
SET(LIBRARY_OUTPUT_DIRECTORY lib)
SET(RUNTIME_OUTPUT_DIRECTORY bin)
SET(ARCHIVE_OUTPUT_DIRECTORY lib)
SET(INCLUDE_OUTPUT_DIRECTORY include)

# Compiler flags
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")




# Configure a header file to pass some of the CMake settings
# to the source code
SET(CONFIG_HPP "config.hpp")
SET(OF_CONFIG ${OF_SOURCE_DIR}/${CONFIG_HPP})
CONFIGURE_FILE(${OF_CONFIG}.in
    ${OF_CONFIG}
    )
SET(SF_CONFIG ${SF_SOURCE_DIR}/${CONFIG_HPP})
CONFIGURE_FILE(${SF_CONFIG}.in
    ${SF_CONFIG}
    )
SET(OF2_CONFIG ${OF2_SOURCE_DIR}/${CONFIG_HPP})
CONFIGURE_FILE(${OF2_CONFIG}.in
    ${OF2_CONFIG}
    )
SET(SF2_CONFIG ${SF2_SOURCE_DIR}/${CONFIG_HPP})
CONFIGURE_FILE(${SF2_CONFIG}.in
    ${SF2_CONFIG}
    )





# Find Packages and other libraries
# opencv boost_system boost_filesystem
FIND_PACKAGE(OpenCV 3.1 REQUIRED COMPONENTS core tracking videoio imgproc highgui optflow)
IF(OpenCV_FOUND)
    SET(OTHER_INCLUDES ${OTHER_INCLUDES} ${OpenCV_INCLUDE_DIR})
    SET(OTHER_LIBS ${OTHER_LIBS} ${OpenCV_LIBS})
ENDIF(OpenCV_FOUND)
message("Opencv dir: ${OpenCV_DIR}")
message("Opencv libs: ${OpenCV_LIBS}")

FIND_PACKAGE(Boost REQUIRED COMPONENTS program_options filesystem system chrono)
IF(Boost_FOUND)
    SET(OTHER_INCLUDES ${OTHER_INCLUDES} ${Boost_INCLUDE_DIRS})
    SET(OTHER_LIBS ${OTHER_LIBS} ${Boost_LIBRARIES})
ENDIF(Boost_FOUND)

FIND_PACKAGE(CUDA REQUIRED)
IF(CUDA_FOUND)
    SET(OTHER_INCLUDES ${OTHER_INCLUDES} ${CUDA_INCLUDE_DIRS})
    SET(OTHER_LIBS ${OTHER_LIBS} ${CUDA_LIBRARIES})
    SET(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} "-arch=sm_20") #-std=c++11")
    message("CUDA found: ${CUDA_INCLUDE_DIRS}")
    message("\t- ${CUDA_LIBRARIES}")
ELSE()
    message("CUDA not found!")
ENDIF()








SET(DESCRIPTOR_LIB "ff_descriptor")
SET(FILE_LIB "ff_file")
SET(OPTICAL_FLOW_LIB "ff_opticalflow")
SET(UTIL_LIB "ff_util")
SET(PDFLOW_LIB "ff_pdflow")
SET(TERMINAL_LIB "ff_terminal")
SET(TIME_LIB "ff_time")
SET(CORE_LIB "ff_core")
SET(DATA_LIB "ff_data")
SET(TRACKER_LIB "ff_tracker")

SET(FILE_DIR ${PROJECT_SOURCE_DIR}/file)
SET(DESCRIPTOR_DIR ${PROJECT_SOURCE_DIR}/descriptor)
SET(UTIL_DIR ${PROJECT_SOURCE_DIR}/util)
SET(OPTICAL_FLOW_DIR ${PROJECT_SOURCE_DIR}/opticalflow)
SET(PDFLOW_DIR ${PROJECT_SOURCE_DIR}/pdflow)
SET(TERMINAL_DIR ${PROJECT_SOURCE_DIR}/terminal)
SET(TIME_DIR ${PROJECT_SOURCE_DIR}/time)
SET(CORE_DIR ${PROJECT_SOURCE_DIR}/core)
SET(DATA_DIR ${PROJECT_SOURCE_DIR}/data)
SET(TRACKER_DIR ${PROJECT_SOURCE_DIR}/tracker)

# Set includes
SET(MY_INCLUDES
    ${MY_INLCUDES}
    ${FILE_DIR}
    ${DESCRIPTOR_DIR}
    ${UTIL_DIR}
    ${OPTICAL_FLOW_DIR}
    ${PDFLOW_DIR}
    ${TERMINAL_DIR}
    ${TIME_DIR}
    ${CORE_DIR}
    ${DATA_DIR}
    ${TRACKER_DIR}
    )


# Set variable for our libraries 
# *_LIB variables defined in subfolders!
SET(MY_LIBS 
    ${MY_LIBS} 
    ${DESCRIPTOR_LIB}
    ${FILE_LIB}
    ${UTIL_LIB}
    ${OPTICAL_FLOW_LIB}
    ${PDFLOW_LIB}
    ${TERMINAL_LIB}
    ${TIME_LIB}
    ${CORE_LIB}
    ${DATA_LIB}
    ${TRACKER_LIB}
    )






# Include
INCLUDE_DIRECTORIES(
    ${OTHER_INCLUDES}
    ${MY_INCLUDES}
    )


# Add subdirectories to get libraries
ADD_SUBDIRECTORY(descriptor)
ADD_SUBDIRECTORY(file)
ADD_SUBDIRECTORY(util)
ADD_SUBDIRECTORY(opticalflow)
ADD_SUBDIRECTORY(pdflow)
ADD_SUBDIRECTORY(terminal)
ADD_SUBDIRECTORY(time)
ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(data)
ADD_SUBDIRECTORY(tracker)



# Add executable
ADD_EXECUTABLE(${OF_BINARY} 
    ${OF_SOURCE_DIR}/main.cpp
    ${OF_SOURCE_DIR}/config.hpp
    )
ADD_EXECUTABLE(${SF_BINARY}
    ${SF_SOURCE_DIR}/main.cpp
    ${SF_SOURCE_DIR}/config.hpp
    )
ADD_EXECUTABLE(${OF2_BINARY} 
    ${OF2_SOURCE_DIR}/main.cpp
    ${OF2_SOURCE_DIR}/config.hpp
    )
ADD_EXECUTABLE(${SF2_BINARY}
    ${SF2_SOURCE_DIR}/main.cpp
    ${SF2_SOURCE_DIR}/config.hpp
    )



# Link librares
TARGET_LINK_LIBRARIES(${OF_BINARY}
    ${OTHER_LIBS}
    ${MY_LIBS}
    )
TARGET_LINK_LIBRARIES(${SF_BINARY}
    ${OTHER_LIBS}
    ${MY_LIBS}
    )
TARGET_LINK_LIBRARIES(${OF2_BINARY}
    ${OTHER_LIBS}
    ${MY_LIBS}
    )
TARGET_LINK_LIBRARIES(${SF2_BINARY}
    ${OTHER_LIBS}
    ${MY_LIBS}
    )



# Add install targets
INSTALL(TARGETS ${OF_BINARY} RUNTIME DESTINATION ${RUNTIME_OUTPUT_DIRECTORY})
INSTALL(TARGETS ${SF_BINARY} RUNTIME DESTINATION ${RUNTIME_OUTPUT_DIRECTORY})
INSTALL(TARGETS ${OF2_BINARY} RUNTIME DESTINATION ${RUNTIME_OUTPUT_DIRECTORY})
INSTALL(TARGETS ${SF2_BINARY} RUNTIME DESTINATION ${RUNTIME_OUTPUT_DIRECTORY})
#INSTALL(FILES "${PROJECT_SOURCE_DIR}/${CONFIG_HPP}" DESTINATION ${INCLUDE_OUTPUT_DIRECTORY})


