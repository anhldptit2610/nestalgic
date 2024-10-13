#!/bin/bash

for test_file in /home/anhld/Projects/nestalgic/65x02/nes6502/v1/*.json; do
    ../build/bin/cpu_test "$test_file"
    if [ $? -ne 0 ]; then
        exit
    fi
done
echo "Congratulation. You passed all the test."
exit