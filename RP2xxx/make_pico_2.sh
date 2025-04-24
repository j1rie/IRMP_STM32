rm ./generated/ws2812.pio.h
mkdir build_pico_2
cd build_pico_2
cmake -D"PICO_BOARD=pico2" ..
make -j5
