#  LIBAOBASFT_FOUND          Set to TRUE if libaobasft librarys and include directory is found
#  LIBAOBASFT_LIBRARY        The libaobasft librarys
#  LIBAOBASFT_INCLUDE_DIR    The libaobasft include directory

find_package(PkgConfig)
pkg_check_modules(PC_LIBAOBASFT QUIET libaobasft)
set(LIBAOBASFT_DEFINITIONS ${PC_LIBAOBASFT_CFLAGS_OTHER})

find_library(LIBAOBASFT_LIBRARY NAMES aobasft libaobasft-0 aobasft-0
				HINTS ${PC_LIBAOBASFT_LIBDIR} ${PC_LIBAOBASFT_LIBRARY_DIRS} ${PROJECT_SOURCE_DIR}/../aoba-libsft/build)

find_path(LIBAOBASFT_INCLUDE_DIR sft/Configuration.hpp
			HINTS ${PC_LIBAOBASFT_INCLUDEDIR} ${PC_LIBAOBASFT_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/../aoba-libsft/include)

if(LIBAOBASFT_LIBRARY AND LIBAOBASFT_INCLUDE_DIR)
    set(LIBAOBASFT_FOUND TRUE)
    message(STATUS "Found libaobasft: ${LIBAOBASFT_LIBRARY}")
else(LIBAOBASFT_LIBRARY AND LIBAOBASFT_PLUGIN_PATH)
    message(STATUS "Could not find libaobasft")
endif(LIBAOBASFT_LIBRARY AND LIBAOBASFT_INCLUDE_DIR)
