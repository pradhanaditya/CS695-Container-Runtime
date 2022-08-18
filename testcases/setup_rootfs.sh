#!/bin/bash

# This script will move some of the testcases files to the rootfs directory
# so that it can be executed from the container

SERVER_FILE="server.py"
CHECK_HOSTNAME_SCRIPT="check_hostname.sh"
CHECK_MEMORY_LIMIT_SCRIPT="check_memory_limit.py"
CHECK_PID_SCRIPT="check_pid_namespace.sh"
CHECK_ROOTFS_SCRIPT="check_rootfs.sh"
ROOTFS_DIR="./rootfs/"
GENERATE_MEMORY_TREASURE="generate_mem_treasure.py"
MEMORY_TREASURE_DIR="./rootfs/memory_treasure"
MEMORY_TREASURE_FILE="memoryTreasure"

python3 $GENERATE_MEMORY_TREASURE
mv $SERVER_FILE $CHECK_HOSTNAME_SCRIPT $CHECK_MEMORY_LIMIT_SCRIPT $CHECK_PID_SCRIPT $CHECK_ROOTFS_SCRIPT $ROOTFS_DIR
mkdir $MEMORY_TREASURE_DIR
mv $MEMORY_TREASURE_FILE $MEMORY_TREASURE_DIR