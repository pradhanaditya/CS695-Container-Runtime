#!/usr/bin/env python3
import sys

memoryTreasureFile = open("/memory_treasure/memoryTreasure", "rb")

fileDataInMemory = ""

nMBToRead = int(input("Enter data (in MB) to read from memory treasure: "))

if (nMBToRead > 500):
    print("Maximum limit is 500 MB")
    exit(1)
else:
    fileDataInMemory = (memoryTreasureFile.read(nMBToRead*1024*1024))
    print("Data read successfully!")
