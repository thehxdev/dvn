add_executable(
    dvn
    main.cpp
)

target_compile_features(dvn PRIVATE cxx_std_20)

target_include_directories(dvn PRIVATE "${RAYLIB_PATH}/include")
target_link_directories(dvn PRIVATE "${RAYLIB_PATH}/lib")

if (MSVC)
    target_link_libraries(dvn PRIVATE raylib gdi32 msvcrt winmm user32 shell32)
else()
    if (WIN32)
        target_link_libraries(dvn PRIVATE raylib m winmm)
    else()
        target_link_libraries(dvn PRIVATE raylib m)
    endif()
endif()
