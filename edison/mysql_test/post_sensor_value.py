#!/usr/bin/env python
from __future__ import print_function
import MySQLdb
import mysql.connector
from datetime import date, datetime, timedelta
import ConfigParser
import sys
import subprocess
import string
import re
import time
import os

add_reading= ("INSERT INTO hw_data " 
                " (house_id, sensor_id, sensor_data, timeofpost) " 
                " VALUES (%s, %s, %s, %s)" )
#data_reading = (house_id, '2', '33.33', datetime.now().date())

config = ConfigParser.ConfigParser()
#values
noise_level=0
co2_level=0
#ids in the db
noise_id=1
co2_id=2
hw_str = ''
noise_level_id = ''
co2_level_id = ''
outfilename='/home/root/bleout.txt'

def scan_and_get_sensor_readings(hw_str):
    noise_level=0
    co2_level=0
    with open(outfilename, "w"):
        pass
    print ("Emptied the output file ", outfilename)

    f = open(outfilename, "w")
    subprocess.call(["blescan", "-n"], stdout=f)
    f.close()
    if (os.stat(outfilename) == 0):
        print("No output captured in %s, sleep and try again\n", outfilename)
        return None, None
    infile=open(outfilename, "r")
    for line in infile:
        words= line.partition(":")
        #print (words[0], " = ", words[2])
        if ((words[2].find(hw_str)) != -1):
        	print ("found our device, get sensor values")
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
            	print ("noise = ", noise_level)
            	print ("co2 = ", co2_level)
                return noise_level, co2_level
                break
        #print words
    infile.close()

def main():
    config.read(sys.argv[1])    
    
    house_id = config.get('Default', 'house_id')
    noise_level_id = config.get('Default', 'noise_level_id')
    co2_level_id = config.get('Default', 'co2_level_id')
    ble_str = config.get('Default', 'ble_string')
    hw_str = ble_str + house_id
    print (hw_str)
    db_host = config.get('Default', 'host')
    db_port = config.get('Default', 'port')
    db_user = config.get('Default', 'user')
    db_passwd = config.get('Default', 'passwd')
    db_name = config.get('Default', 'db')
    
    cnx = mysql.connector.connect(host=db_host, port=db_port, user=db_user,passwd=db_passwd, database=db_name)
    cursor = cnx.cursor()
    #test
    #show_dbs(cursor)
    # get data and post it to the table
    while (1):
        noise_level, co2_level = scan_and_get_sensor_readings(hw_str)  or (None,None)
        if noise_level is None:
            print("Found no noise sensor data\n")
            time.sleep(5)
            continue
        if co2_level is None:
            print("Found no co2 sensor data\n")
            time.sleep(5)
            continue
        n = int(noise_level, 16) 
        c = int(co2_level,16 )
        #we reduced the the noise to 1 byte to fit easily in the beacon
        noise_calibrated = n * 16 
        co2_calibrated = c * 16 
        print("noise = ", noise_calibrated)
        print("co2 = ", co2_calibrated)
        data_reading = (house_id, noise_id, noise_calibrated, datetime.now())
        cursor.execute(add_reading, data_reading)
        data_reading = (house_id, co2_id, co2_calibrated, datetime.now())

        cursor.execute(add_reading, data_reading)
        #print_table("hw_data")
        cnx.commit()

    cursor.close()
    cnx.close()

def show_dbs(cursor):
    query= ("show databases")
    cursor.execute(query)
    row = cursor.fetchone()
    while row is not None:
        print(row)
        row = cursor.fetchone()



if __name__ == '__main__':
    main()
