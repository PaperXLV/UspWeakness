add_library(usplib usp.cpp uspgenerator.cpp)
target_include_directories(usplib PUBLIC /)
target_link_libraries(
  usplib 
  PRIVATE project_options
          project_warnings
          CONAN_PKG::docopt.cpp
          CONAN_PKG::fmt
          CONAN_PKG::spdlog)

add_executable(usp main.cpp)
target_link_libraries(
  usp
  PRIVATE usplib
          project_options
          project_warnings
          CONAN_PKG::docopt.cpp
          CONAN_PKG::fmt
          CONAN_PKG::spdlog)