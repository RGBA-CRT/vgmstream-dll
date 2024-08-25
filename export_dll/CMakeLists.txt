# public export dll
set(PJNAME vgmstream-dll)

set(SRCS public_dll.c CException/lib/CException.c)
if(WIN32)
	set(SRCS ${SRCS} vgmstream.def)
endif()
add_library(${PJNAME} SHARED ${SRCS})

set_target_properties(${PJNAME} PROPERTIES
	PREFIX ""
	OUTPUT_NAME "vgmstream")

# Link to the vgmstream library
target_link_libraries(${PJNAME} libvgmstream)

# Link to Jansson, if we have it
if (USE_JANSSON)
    target_compile_definitions(${PJNAME} PRIVATE HAVE_JSON)
	target_include_directories(${PJNAME} PRIVATE ${JANSSON_INCLUDE_DIRS})
	target_link_libraries(${PJNAME} jansson)
endif()

setup_target(${PJNAME} TRUE)

if(WIN32)
	# Make sure that the binary directory is included (for version.h), as well as the getopt library include directory
	target_include_directories(${PJNAME} PRIVATE
		${VGM_BINARY_DIR})

	# Include the version string
	if(MSVC)
		add_dependencies(${PJNAME} version_h)
	elseif(MINGW)
		if(VGMSTREAM_VERSION)
			target_compile_definitions(${PJNAME} PRIVATE VGMSTREAM_VERSION="${VGMSTREAM_VERSION}")
		endif()

		# Also, on MinGW when using GCC, these flags need to be included to prevent requiring MinGW's runtime DLLs from being included, which does unfortunately increase the size of the EXE
		if(NOT CMAKE_CXX_COMPILER_ID MATCHES Clang)
			set_target_properties(${PJNAME} PROPERTIES
				LINK_FLAGS "-static-libgcc -static-libstdc++ -Wl,--kill-at -funwind-tables -fexceptions")
		endif()
	endif()

	# Install the DLLs
	install_dlls(${CMAKE_INSTALL_PREFIX}/bin)
elseif(VGMSTREAM_VERSION)
	# Include the version string
	target_compile_definitions(${PJNAME} PRIVATE VGMSTREAM_VERSION="${VGMSTREAM_VERSION}")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID MATCHES GNU)
	set_target_properties(${PJNAME} PROPERTIES LINK_FLAGS_RELEASE -s)
endif()

# Install the CLI program
install(TARGETS ${PJNAME}
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
