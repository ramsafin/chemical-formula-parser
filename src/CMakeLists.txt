add_library(${PROJECT_NAME} STATIC
  library.cpp
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Forcing C++20 standard
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

enable_strict_warnings(${PROJECT_NAME})
enable_sanitizers(${PROJECT_NAME})
enable_coverage(${PROJECT_NAME})
