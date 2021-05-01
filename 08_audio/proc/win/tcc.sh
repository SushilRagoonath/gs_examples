#!bin/sh

rm -rf bin
mkdir bin
cd bin

proj_name=App
proj_root_dir=$(pwd)/../

flags=(
  -std=c99 
)

# Include directories
inc=(
	-I ../../third_party/include/			# Gunslinger includes
)

# Source files
src=(
	../source/main.c
)

libs=(
 	-lopengl32
 	-lkernel32
 	-luser32
 	-lshell32
 	-lgdi32
 	-lAdvapi32
 	-lmsvcrt
 	-lwinmm
)

# Build
tcc ${inc[*]} ${src[*]} ${flags[*]} ${libs[*]}  -o ${proj_name}.exe -m32 -DSTBI_NO_SIMD -bench -D_wfindfirst32i64=_wfindfirsti64 -D_wfindnext32i64=_wfindnexti64 -DMA_NO_WASAPI,-subsystem=gui
cd ..



