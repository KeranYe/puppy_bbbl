#!/bin/sh
# 2019-08-16 LLW shell script for changing ownership and sticky bit for puppy_bbbl
# usage: ~/bin/edumip_change_perms.sh
#
echo ls -l ~/catkin_ws/devel/lib/puppy_bbbl/puppy_bbbl_static_test_v1
ls -l ~/catkin_ws/devel/lib/puppy_bbbl/puppy_bbbl_static_test_v1

echo sudo chown root:root  ~/catkin_ws/devel/lib/puppy_bbbl/puppy_bbbl_static_test_v1
sudo chown root:root  ~/catkin_ws/devel/lib/puppy_bbbl/puppy_bbbl_static_test_v1

echo sudo chmod u+s  ~/catkin_ws/devel/lib/puppy_bbbl/puppy_bbbl_static_test_v1
sudo chmod u+s  ~/catkin_ws/devel/lib/puppy_bbbl/puppy_bbbl_static_test_v1

echo ls -l ~/catkin_ws/devel/lib/puppy_bbbl/puppy_bbbl_static_test_v1
ls -l ~/catkin_ws/devel/lib/puppy_bbbl/puppy_bbbl_static_test_v1
