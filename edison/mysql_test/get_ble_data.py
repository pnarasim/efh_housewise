#!/usr/bin/python
import subprocess
import string
import re

hw_str = "HW-ID2"

#scan_result = subprocess.check_output(["blescan", "-n"], stderr=subprocess.STDOUT)
#print scan_result

f = open("bleout.txt", "w")
subprocess.call(["blescan", "-n"], stdout=f)
f.close()
infile=open("bleout.txt", "r")
for line in infile:
    words= line.partition(":")
    #print (words[0], " = ", words[2])
    if ((words[2].find(hw_str)) != -1):
        print "found our device, get sensor values"
        print (words[2])
        # get the next 2 lines, they have the sensor values
        s1 = infile.next()
        s2 = infile.next()
        words = s1.partition(":")
        if (words[0].find("0x22") != -1):
            sensor_level = words[2]
            noise_level = re.sub('[\W]+', '', sensor_level)
        words = s2.partition(":")
        if (words[0].find("0x33") != -1):
            sensor_level = words[2]
            co2_level = re.sub('[\W]+', '', sensor_level)
        print s1
        print s2
        print noise_level
        print co2_level
    #print words
infile.close()


