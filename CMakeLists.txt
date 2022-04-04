cmake_minimum_required(VERSION 3.12)
project(OcrEngine VERSION 0.0.1 DESCRIPTION "Use tesseract for medical document analysis")

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(TESSERACT REQUIRED tesseract)
pkg_search_module(LEPTONICA REQUIRED lept)

# Make sure C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set useful compiler warnings
add_compile_options(-Wall -Wextra -pedantic)

# Enable pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

######################
# Define the library #
######################
set(LIB_DIRECTORY ocr)
set(LIB_NAME OcrLib)
file(GLOB LIB_SOURCES ${LIB_DIRECTORY}/*.cpp ${LIB_DIRECTORY}/*.hpp)
add_library(${LIB_NAME} SHARED ${LIB_SOURCES})
set_target_properties(${LIB_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
target_include_directories(${LIB_NAME} INTERFACE ${LIB_DIRECTORY})

# Add link to thread support for the library
target_link_libraries(${LIB_NAME} INTERFACE Threads::Threads)

# Add Tesseract and its Leptonica dependency 
target_include_directories(${LIB_NAME} PRIVATE ${TESSERACT_INCLUDE_DIRS})
target_link_libraries(${LIB_NAME} INTERFACE ${TESSERACT_LIBRARIES})
target_include_directories(${LIB_NAME} PRIVATE ${LEPTONICA_INCLUDE_DIRS})
target_link_libraries(${LIB_NAME} INTERFACE ${LEPTONICA_LIBRARIES})

# Add OpenCV
target_include_directories(${LIB_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${LIB_NAME} INTERFACE ${OpenCV_LIBS})

# Add logging
set(LOG_LIB_DIRECTORY third-party/easyloggingpp)
set(LOG_LIB_NAME easyloggingpp)
file(GLOB LOG_LIB_SOURCES ${LOG_LIB_DIRECTORY}/*.cc ${LOG_LIB_DIRECTORY}/*.h)
add_library(${LOG_LIB_NAME} SHARED ${LOG_LIB_SOURCES})
target_include_directories(${LIB_NAME} PUBLIC ${LOG_LIB_DIRECTORY})
target_link_libraries(${LIB_NAME} PUBLIC ${LOG_LIB_NAME})

###########################
# Define the applications #
###########################
set(APP_DIRECTORY application)
set(APP_NAME ocrize)
file(GLOB APP_SOURCES ${APP_DIRECTORY}/*.cpp)
add_executable(${APP_NAME} ${APP_SOURCES})

# Add ocr engine library
target_link_libraries(${APP_NAME} PRIVATE ${LIB_NAME})

# Add argparse
set(ARG_LIB_DIRECTORY third-party/argparse)
target_include_directories(${APP_NAME} PRIVATE ${ARG_LIB_DIRECTORY})

# Add json
set(JSON_LIB_DIRECTORY third-party/nlohmann_json)
target_include_directories(${APP_NAME} PRIVATE ${JSON_LIB_DIRECTORY})

####################
# Define the tests #
####################
# Enable testing
enable_testing()
# Define the test application
set(TEST_LIB_DIRECTORY third-party/catch2)
set(TEST_SOURCE_DIRECTORY tests)
set(TEST_EXE ocrUnitTest)
file(GLOB TEST_SOURCES ${TEST_SOURCE_DIRECTORY}/*.cpp)
add_executable(${TEST_EXE} ${TEST_SOURCES})
# Add library link to test application
target_link_libraries(${TEST_EXE} ${LIB_NAME})
target_include_directories(${TEST_EXE} PRIVATE ${TEST_LIB_DIRECTORY})

# Add unit tests
add_test(searchLibUnitTest ${TEST_EXE})
