#/bin/sh
for i in $(seq 0 199); do
#    diff -sq test${i} test$((i+1))
#    diff -sq test${i} testreference
    diff -c test${i} testreference
done
