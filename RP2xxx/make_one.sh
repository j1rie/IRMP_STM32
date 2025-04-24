rm ./generated/ws2812.pio.h
mkdir build_one
cd build_one
cmake -D"PICO_BOARD=waveshare_rp2040_one" ..
make -j5
