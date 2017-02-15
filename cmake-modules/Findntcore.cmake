find_path(ntcore_INCLUDE_DIR NAMES libfreenect2.hpp
        HINTS
        /usr/local/include/llvm/
        /usr/local/include/support/
        /usr/local/include/tcpsockets/
        /usr/local/include/
        /usr/include/llvm/
        /usr/include/support/
        /usr/include/tcpsockets/
        /usr/include/
        }
)
 
find_library(ntcore_LIBRARY NAMES ntcore wpiutil.a)

if(ntcore_INCLUDE_DIR AND ntcore_LIBRARY)
  set(ntcore_FOUND TRUE)
endif()

if(ntcore_LIBRARY)
    set(ntcore_LIBRARY ${ntcore_LIBRARY})
endif()

if (ntcore_FOUND)
  mark_as_advanced(ntcore_INCLUDE_DIR ntcore_LIBRARY ntcore_LIBRARIES)
endif()