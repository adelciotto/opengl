#!/bin/bash

set -u

release_mode=false
demo=
while getopts r:d: flag
do
    case "${flag}" in
        r) $release_mode=true;;
        d) demo=${OPTARG};;
    esac
done

demos=(
    triangle
)

if [[ ! ${demos[@]} =~ $demo ]]; then
    echo "$demo is not a valid demo program"
    exit -1
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

echo "building $demo in bin directory..."
echo
rm -rf bin
mkdir bin
cd bin

srcs=(
	../src/sdl2_impl/gl.c
	../src/sdl2_impl/glut.c
)

(set -x; gcc ${includes[*]} ${cflags[*]} ${srcs[*]} "../demo/$demo.c" ${libs[*]} -o "${demo}_${exec_suffix}")
exitCode=$?

echo
echo "build complete!"

cd ..
exit $exitCode
