add_library(${PROJECT_NAME} STATIC
  token.cpp
  tokenizer.cpp
  parser.cpp
  error/parser_error.cpp
  error/tokenizer_error.cpp
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Forcing CMAKE_CXX_STANDARD standard on the library
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_${CMAKE_CXX_STANDARD})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

enable_strict_warnings(${PROJECT_NAME})
enable_sanitizers(${PROJECT_NAME})
enable_coverage(${PROJECT_NAME})
