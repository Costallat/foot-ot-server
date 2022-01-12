#!/bin/bash
# Help                                                     #
############################################################
############################################################
Help()
{
   # Display Help
   echo "Add description of the script functions here."
   echo
   echo "Syntax: scriptTemplate [-g|h|v|p|V]"
   echo "options:"
   echo "a     Install packages, vcpkg, build server"
   echo "b     Build Server."
   echo "c     Clean removing build folder."
   echo "h     Print this Help."
   echo "p     Install Packages."
   echo "v     Install vcpkg."
   echo
}

# Install Packages                                         #
############################################################
############################################################
InstallPackages()
{
	sudo apt-get update
	sudo apt-get install git cmake build-essential libluajit-5.1-dev ca-certificates curl zip unzip tar pkg-config ninja-build
}

# Install vcpkg                                            #
############################################################
############################################################
InstallVcpkg()
{
  cd ~ || exit
  git clone https://github.com/microsoft/vcpkg
  cd vcpkg || exit
  ./bootstrap-vcpkg.sh
  cd "$PWD_SCRIPT" || exit
}

# Build Server                                             #
############################################################
############################################################
BuildServer()
{
  mkdir -p build
  cd build ||exit
  cmake -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake ..
  make -j`nproc`
}

# Clean                                                    #
############################################################
############################################################
Clean()
{
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
while getopts ":abchpv:" option; do
  case $option in
    a) #Do all
      InstallPackages
      InstallVcpkg
      BuildServer
      ;;
    b) # Install packages
      BuildServer
      ;;
    c) # Clean
      Clean
      ;;
    h) # display Help
      Help
      exit;;
    p) # Install packages
      InstallPackages
      ;;
    v) # Install vcpkg
      InstallVcpkg
      ;;
    :) # Default
      Help
      ;;
    \?) # Invalid option
      echo "Error: Invalid option"
      exit;;
  esac
done

