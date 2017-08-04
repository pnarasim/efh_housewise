from __future__ import print_function
import MySQLdb
import mysql.connector
from datetime import date, datetime, timedelta

host="54.245.167.125"
port=3306
user="hw1"
passwd="efh@2017"
db="housewise"

add_reading= ("INSERT INTO hw_data " 
                " (house_id, sensor_id, sensor_value, timeofpost) " 
                " VALUES (%s, %s, %s, %s)" )
data_reading = ('133', '2', '33.33', datetime.now().date())

cnx = mysql.connector.connect(host=host, port=port, user=user,passwd=passwd, database=db)
cursor = cnx.cursor()

query= ("show databases")
cursor.execute(query)
row = cursor.fetchone()
while row is not None:
        print(row)
        row = cursor.fetchone()

cursor.execute(add_reading, data_reading)
cnx.commit()

query = ("Select * from hw_data")
cursor.execute(query)
row = cursor.fetchone()
while row is not None:
        print(row)
        row = cursor.fetchone()

cnx.commit()
cursor.close()
cnx.close()

