#/bin/sh

alias nproc='sysctrl -n hw.ncpu'

mkdir build
cd build
rm -rf *

# NOTE(jda) - using Internal tasking system here temporarily to avoid installing TBB
cmake \
-D OSPRAY_TASKING_SYSTEM=Internal \
..

make -j`nproc`
