#!/bin/bash
rfkill unblock bluetooth

/home/root/mysql_test/post_sensor_value.py /home/root/mysql_test/housewise.cfg > /home/root/cronlog.txt 2>&1


