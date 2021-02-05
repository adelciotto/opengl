#!/bin/bash

release_mode=false
while getopts r:d: flag
do
    case "${flag}" in
        r) $release_mode=true;;
        d) demo=${OPTARG};;
    esac
done

if [[ ${array[@]} =~ $value ]]; then
    # whatever you want to do when array contains value
fi

exec_suffix="debug"

cflags=(
    -std=c99 -Wall -Wextra $(eval pkg-config --cflags sdl2)
)

includes=(
	-I ../include/
)

libs=(
    -lm $(eval pkg-config --libs sdl2)
)

if [ $release_mode = true ]; then
    $exec_suffix="release"
    cflags+=("-03" "-DNDEBUG")
else
    cflags+=("-g" "-DDEBUG")
fi

echo "building executables in bin directory..."
echo
rm -rf bin
mkdir bin
cd bin

demos=(
    triangle
)

srcs=(
	../src/sdl2_impl/gl.c
	../src/sdl2_impl/glut.c
)

# TODO: Build program supplied by cli arg
for demo in ${demos[@]}
do
    (set -x; gcc ${includes[*]} ${cflags[*]} ${srcs[*]} "../demo/$demo.c" ${libs[*]} -o "${demo}_${exec_suffix}")
done

echo
echo "build complete!"

cd ..
