#!/bin/bash

# This script will display the output of 'ps aux' command and check whether the shell is
# running with pid 1 or not

echo "Output of 'ps aux' command:"
ps aux
CHECK_BASH_PID=`ps aux | grep "bash" | awk 'FNR == 1 {print $2}'`

if [ $CHECK_BASH_PID -eq 1 ]
then
    echo "Test passed : 'ps aux' command is displaying the output as expected"
    exit 0
else
    echo "Test failed : bash is not running with pid = 1"
    exit 1
fi