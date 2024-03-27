mkdir build_pico
cd build_pico
cmake -D"PICO_BOARD=pico" ..
make -j5