#!/bin/bash

# This script will check whether the hostname of the container is set up as expected
# or not.

if [ $# -eq 1 ]
then
    EXPECTED_HOSTNAME=$1
    ACTUAL_HOSTNAME=`hostname`

    if [ "$EXPECTED_HOSTNAME" = "$ACTUAL_HOSTNAME" ]
    then
        echo "Test passed : Actual hostname is same as the expected hostname"
        exit 0
    else
        echo "Test failed : Actual hostname is \"$ACTUAL_HOSTNAME\""
        exit 1
    fi
else
    echo "Usage : ./check_hostname [EXPECTED HOSTNAME]"
    exit 1
fi