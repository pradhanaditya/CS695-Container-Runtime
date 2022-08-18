#!/bin/bash

# This script will check whether the output of "ls" and "ls /" command
# is same or not. If it is same, it implies that the shell begins the
# execution in the top-level directory of root filesystem.

LS_OUTPUT=`ls`
LS_ROOT_OUTPUT=`ls /`

if [ "$LS_OUTPUT" = "$LS_ROOT_OUTPUT" ]
then
    echo "Test passed : Shell begins the execution in the top-level directory of root filesystem"
    exit 0
else
    echo "Test failed : Shell does not begin the execution in the top-level directory of root filesystem"
    exit 1
fi