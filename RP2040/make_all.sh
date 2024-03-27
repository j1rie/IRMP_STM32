mkdir build_pico
cd build_pico
cmake -D"PICO_BOARD=pico" ..
make -j5
cd ..
mkdir build_one
cd build_one
cmake -D"PICO_BOARD=waveshare_rp2040_one" ..
make -j5
cd ..
mkdir build_zero
cd build_zero
cmake  -D"PICO_BOARD=waveshare_rp2040_zero" ..
make -j5
cd ..
