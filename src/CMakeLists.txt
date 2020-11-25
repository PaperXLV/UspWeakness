# Generic test that uses conan libs
add_executable(usp main.cpp usp.cpp)
target_link_libraries(
  usp
  PRIVATE project_options
          project_warnings
          CONAN_PKG::docopt.cpp
          CONAN_PKG::fmt
          CONAN_PKG::spdlog)