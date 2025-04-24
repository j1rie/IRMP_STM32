rm ./generated/ws2812.pio.h
mkdir build_seeed_xiao_rp2350
cd build_seeed_xiao_rp2350
cmake -D"PICO_BOARD=seeed_xiao_rp2350" ..
make -j5
cd ..
