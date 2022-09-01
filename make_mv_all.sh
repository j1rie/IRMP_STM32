#! /bin/bash
cd STM32F103x8
./make_all.sh
./mv_all.sh
cd ..
cd STM32F105xC
./make_all.sh
./mv_all.sh
cd ..
cd STM32F303xC
./make_all.sh
./mv_all.sh
cd ..
cd STM32F401xC
./make_all.sh
./mv_all.sh
cd ..
cd STM32F401xE
./make_all.sh
./mv_all.sh
cd ..
