cmake_minimum_required(VERSION 3.5)
project(RecipeOrganizer)

set(CMAKE_C_STANDARD 11)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_executable(RecipeOrganizer src/main.c)
    message("This is a Windows System")
    set(GTK3_INCLUDE_DIRS "C:/msys64/mingw64/include/")
    set(GTK3_LIBRARY_DIRS "C:/msys64/mingw64/lib")
    set(ENV{PKG_CONFIG_PATH} "C:/msys64/mingw64/lib/pkgconfig")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK4 REQUIRED gtk4)

    find_package(SQLite3 REQUIRED)
    pkg_check_modules(SQLITE REQUIRED sqlite3)

    include_directories(${GTK4_INCLUDE_DIRS})
    link_directories(${GTK4_LIBRARY_DIRS})
    add_definitions(${GTK4_CFLAGS_OTHER})

    set_target_properties(RecipeOrganizer PROPERTIES WIN32_EXECUTABLE true)  # If you dont want console to run with your .exe
    target_link_libraries(RecipeOrganizer ${GTK4_LIBRARIES})
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_executable(RecipeOrganizer src/main.c)
    message("This is a Linux System LETS GOOOO")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK REQUIRED gtk4)

    find_package(SQLite3 REQUIRED)
    pkg_check_modules(SQLITE REQUIRED sqlite3)

    include_directories(${GTK_INCLUDE_DIRS})
    link_directories(${GTK_LIBRARY_DIRS})

    # Link the target to the GTK+ libraries
    TARGET_LINK_LIBRARIES(RecipeOrganizer sqlite3 ${GTK_LIBRARIES})
endif()
