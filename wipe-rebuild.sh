#!/bin/bash

function print_usage(){
    cat<<END
Usage: $0 [-h]
    This program wips all the cmake generated files, then
rebuild with Fedora build enabled.
END
}

if [ "$1" = "-h" ];then
    print_usage
    exit 0
fi

find . -path "*/CMakeFiles/*" -print -delete
find . -name "Makefile" -print -delete
rm -fv CMakeCache.txt
rm -fv cmake_install.cmake
rm -fv cmake_uninstall.cmake
rm -fv CPackConfig.cmake
rm -frv _CPack_Packages
rm -fv CPackSourceConfig.cmake
rm -fv CTestTestfile.cmake
rm -frv NO_PACK
rm -frv Testing


##== Rebuild
##=== CMake options
OptArray=()

if which lsb_release &>/dev/null ;then
    Distributor=$(lsb_release -i | sed -e 's/^.*:\s*//')
    Release=$(lsb_release -r | sed -e 's/^.*:\s*//')
    case $Distributor in
	RedHat* )
	    OptArray+=(-DCMAKE_FEDORA_ENABLE_FEDORA_BUILD=1 -DGSETTINGS_SUPPORT=0 -DGCONF2_SUPPORT=1)
	    ;;
	Fedora )
	    OptArray+=(-DCMAKE_FEDORA_ENABLE_FEDORA_BUILD=1 -DGSETTINGS_SUPPORT=1 -DGCONF2_SUPPORT=0)
	    ;;
	* )
	    OptArray+=(-DGSETTINGS_SUPPORT=1)
	    ;;
    esac
else
    OptArray+=(-DGSETTINGS_SUPPORT=1)
fi

cmake "${OptArray[@]}" .
