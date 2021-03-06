if(${ENABLE_libefp})
    find_package(libefp 1.5.0 CONFIG QUIET COMPONENTS shallow)

    if(${libefp_FOUND})
        get_property(_loc TARGET libefp::efp PROPERTY LOCATION)
        message(STATUS "${Cyan}Found libefp${ColourReset}: ${_loc} (found version ${libefp_VERSION})")
        add_library(libefp_external INTERFACE)  # dummy
    else()
        if(${CMAKE_INSIST_FIND_PACKAGE_libefp})
            message(FATAL_ERROR "Suitable libefp could not be externally located as user insists")
        endif()

        include(ExternalProject)
        message(STATUS "Suitable libefp could not be located, ${Magenta}Building libefp${ColourReset} instead.")
        ExternalProject_Add(libefp_external
            DEPENDS lapack_external
            GIT_REPOSITORY https://github.com/ilyak/libefp
            GIT_TAG 1.5.0  # 4cbc84c
            UPDATE_COMMAND ""
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                       -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                       -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                       -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}
                       -DCMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR}
                       -DCMAKE_INSTALL_DATADIR=${CMAKE_INSTALL_DATADIR}
                       -DCMAKE_INSTALL_INCLUDEDIR=${CMAKE_INSTALL_INCLUDEDIR}
                       -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
                       -DENABLE_OPENMP=OFF  # Psi4 sometimes reacts poorly to threaded efp
                       -DENABLE_XHOST=${ENABLE_XHOST}
                       -DBUILD_FPIC=${BUILD_FPIC}
                       -DENABLE_GENERIC=${ENABLE_GENERIC}
                       -DLIBC_INTERJECT=${LIBC_INTERJECT}
                       -DINSTALL_DEVEL_HEADERS=ON
                       -DFRAGLIB_UNDERSCORE_L=OFF
                       -DFRAGLIB_DEEP=OFF
                       -DTargetLAPACK_DIR=${STAGED_INSTALL_PREFIX}/share/cmake/TargetLAPACK
            CMAKE_CACHE_ARGS -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
            INSTALL_COMMAND DESTDIR=${CMAKE_BINARY_DIR}/stage ${CMAKE_MAKE_PROGRAM} install)

        set(libefp_DIR ${STAGED_INSTALL_PREFIX}/share/cmake/libefp CACHE PATH "path to internally built libefpConfig.cmake" FORCE)
    endif()
else()
    add_library(libefp_external INTERFACE)  # dummy
endif()
