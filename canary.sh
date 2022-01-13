#!/bin/bash
# Help                                                     #
############################################################
############################################################
Help() {
  # Display Help
  echo "Arguments must follow the order."
  echo
  echo "Syntax: scriptTemplate [-a|b|c|d|h|p|v]"
  echo "options:"
  echo "a     Install packages, vcpkg, clean and build server"
  echo "c     Clean removing build folder."
  echo "p     Install Packages."
  echo "v     Install vcpkg."
  echo "b     Build Server."
  echo "d     Build docker image"
  echo "h     Print this Help."
  echo
}

############################################################
############################################################
# Install Packages                                         #
############################################################
############################################################
InstallPackages() {
  sudo apt-get update
  sudo apt-get install git cmake build-essential libluajit-5.1-dev ca-certificates curl zip unzip tar pkg-config ninja-build
}

############################################################
############################################################
# Install vcpkg                                            #
############################################################
############################################################
InstallVcpkg() {
  cd ~ || exit
  git clone https://github.com/microsoft/vcpkg
  cd vcpkg || exit
  ./bootstrap-vcpkg.sh
  cd "$PWD_SCRIPT" || exit
}

############################################################
############################################################
# Build Server                                             #
############################################################
############################################################
BuildServer() {
  mkdir -p build
  cd build || exit
  cmake -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake ..
  make -j$(nproc)
}

############################################################
############################################################
# Build Docker                                             #
############################################################
############################################################
BuildDocker() {
  docker build -f docker/server/Dockerfile -t canary:latest .
}

############################################################
############################################################
# Clean                                                    #
############################################################
############################################################
Clean() {
  rm -rf build
}

############################################################
############################################################
# Main program                                             #
############################################################
############################################################

# Set variables
PWD_SCRIPT=$(pwd)

############################################################
# Process the input options. Add options as needed.        #
############################################################
# Get the options

while getopts "acpvbdh" option; do
  case $option in
  a) #Do all
    InstallPackages
    InstallVcpkg
    Clean
    BuildServer
    ;;
  c) # Clean
    Clean
    ;;
  p) # Install packages
    InstallPackages
    ;;
  v) # Install vcpkg
    InstallVcpkg
    ;;
  b) # Build server
    BuildServer
    ;;
  d) # Build Docker Image
    BuildDocker
    ;;
  h) # display Help
    Help
    exit
    ;;
  \?) # Invalid option
    echo "Error: Invalid option"
    exit
    ;;
  esac
done

if ((OPTIND == 1)); then
  Help
fi
