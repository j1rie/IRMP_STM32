mkdir build_zero
cd build_zero
cmake  -D"PICO_BOARD=waveshare_rp2040_zero" ..
make -j5
