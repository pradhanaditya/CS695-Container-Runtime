#!/usr/bin/env python3

uRandomFile = open("/dev/urandom", "rb")

memoryTreasureFile = open("./memoryTreasure", "wb")

sizeOfDataToRead = int(input("Size of Memory Treasure (in MB): "))

memoryTreasureFile.write((uRandomFile.read(sizeOfDataToRead*1024*1024)))