#!/usr/bin/zsh

# Get project name from CMakeLists.txt
project_name=$(grep -oP '(?<=project\()\w+' CMakeLists.txt)

# Get the project name
if [[ $project_name ]]; then
  echo $project_name
else
  echo "Project name not found in CMakeLists.txt"
fi

# .clang-format and .clang-tidy
if [[ ! -f .clang-format ]]; then
    clang-format --style Webkit --dump-config > .clang-format 
fi

if [[ ! -f .clang-tidy ]]; then
    clang-tidy --dump-config > .clang-tidy
fi

# Create build directory
if [[ ! -d ./build/ ]]; then
  echo "creating build directory"
  mkdir build
fi

# First time? Generate ninja build files using cmake
if [[ -d ./build ]] && [[ $(ls ./build | wc -l ) -eq 0 ]]; then
  cmake -GNinja -Bbuild -DCMAKE_BUILD_TYPE=Debug
fi

if [[ ! -f compile_commands.json ]]; then
    ln -s ./build/compile_commands.json compile_commands.json
fi

cmake --build build
./build/server
