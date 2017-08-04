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
#data_reading = (house_id, '2', '33.33', datetime.now().date())

config = ConfigParser.ConfigParser()

def main():
    config.read(sys.argv[1])    
    house_id = config.get('Default', 'house_id')
    db_host = config.get('Default', 'host')
    db_port = config.get('Default', 'port')
    db_user = config.get('Default', 'user')
    db_passwd = config.get('Default', 'passwd')
    db_name = config.get('Default', 'db')
    
    cnx = mysql.connector.connect(host=db_host, port=db_port, user=db_user,passwd=db_passwd, database=db_name)
    cursor = cnx.cursor()
    #test
    show_dbs(cursor)
    # get data and post it to the table

    #cursor.execute(add_reading, data_reading)
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
    query = ("select * from "
            " (%s) " )
    cursor.execute(query, tablename)
    row = cursor.fetchone()
    while row is not None:
        print(row)
        row = cursor.fetchone()




if __name__ == '__main__':
    main()
