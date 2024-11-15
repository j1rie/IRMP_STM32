test_snd_rcv_all_loop.sh sends a set of infrared codes 200 times and stm32IRconfig 
in test mode receives them and writes them to files.
compare_testfiles.sh compares these files with a reference file and shows the differences.

You will need irctl from https://github.com/olebowle/irctl

You will only get error-free test results under good conditions.
Daylight is best, a weak standard light bulb is already worse, and a strong LED light is even more worse.
The distance and angle from the transmitting LED to the TSOP will also affect the result. Too close to a strong transmitter is bad.
With identical TSOPs under difficult conditions one lost more and different protocols than the other.

To receive all protocols, the IR receiver must be able to decode short bursts/gaps. I used a TSOP 34338.
