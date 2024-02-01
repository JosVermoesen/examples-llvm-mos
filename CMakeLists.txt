cmake_minimum_required(VERSION 3.18)
add_subdirectory(tools)
set(LLVM_MOS_PLATFORM rp6502)
find_package(llvm-mos-sdk REQUIRED)
project(MY-RP6502-PROJECT)

add_executable(mandelbrot)
rp6502_executable(mandelbrot)
target_sources(mandelbrot PRIVATE
    src/mandelbrot.c
)

add_executable(palette)
rp6502_executable(palette)
target_sources(palette PRIVATE
    src/palette.c
)
