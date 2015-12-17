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
rm -f rpm/SPECS/ibus-chewing.spec


##== Rebuild
##=== CMake options
OptArray=()

if which lsb_release &>/dev/null ;then
    Distributor=$(lsb_release -i | sed -e 's/^.*:\s*//')
    Release=$(lsb_release -r | sed -e 's/^.*:\s*//')
    MajorRelease=$(sed -e 's/\..*//' <<< $Release)
    case $Distributor in
	RedHat* )
	    if [ $MajorRelease -ge 7 ];then
		OptArray+=(-DCMAKE_FEDORA_ENABLE_FEDORA_BUILD=ON -DGSETTINGS_SUPPORT=ON -DGCONF2_SUPPORT=OFF)
	    else
		OptArray+=(-DCMAKE_FEDORA_ENABLE_FEDORA_BUILD=ON -DGSETTINGS_SUPPORT=OFF -DGCONF2_SUPPORT=ON)
	    fi
	    ;;
	Fedora )
	    OptArray+=(-DCMAKE_FEDORA_ENABLE_FEDORA_BUILD=ON -DGSETTINGS_SUPPORT=ON -DGCONF2_SUPPORT=OFF)
	    ;;
	* )
	    OptArray+=(-DGSETTINGS_SUPPORT=ON)
	    ;;
    esac
else
    OptArray+=(-DGSETTINGS_SUPPORT=1)
fi

cmake "${OptArray[@]}" .
