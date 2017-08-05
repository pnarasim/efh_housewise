#!/usr/bin/env python
from __future__ import print_function
import MySQLdb
import mysql.connector
from datetime import date, datetime, timedelta
import ConfigParser
import sys


add_reading= ("INSERT INTO hw_data " 
                " (house_id, sensor_id, sensor_value, timeofpost) " 
                " VALUES (%s, %s, %s, %s)" )

config = ConfigParser.ConfigParser()

def read_meter_from_file(meter_reading_file):
	fh = open(meter_reading_file, "r")
	r1 = fh.read()
	fh.close()	
	return r1 

def main():
    config.read(sys.argv[1])    
    house_id = config.get('Default', 'house_id')
    sensor_id = config.get('Default', 'sensor_id')
    db_host = config.get('Default', 'host')
    db_port = config.get('Default', 'port')
    db_user = config.get('Default', 'user')
    db_passwd = config.get('Default', 'passwd')
    db_name = config.get('Default', 'db')
    meter_reading_file = config.get('Default', 'meter_file')

    cnx = mysql.connector.connect(host=db_host, port=db_port, user=db_user,passwd=db_passwd, database=db_name)
    cursor = cnx.cursor()
    #test
    show_dbs(cursor)
    # get data and post it to the table
    meter_reading=read_meter_from_file(meter_reading_file)
    data_reading = (house_id, sensor_id, meter_reading, datetime.now().date())

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

#tbd : fix this syntax
def print_table( tablename ):
    "This prints the rows in this table"
    query = ("select * from hw_data")
    cursor.execute(query, tablename)
    row = cursor.fetchone()
    while row is not None:
        print(row)
        row = cursor.fetchone()




if __name__ == '__main__':
    main()
