#!/usr/bin/env python

from epos2.srv import *
import rospy
import sys
from std_msgs.msg import String
from subprocess import call


def request_velocity(velocity):
    rospy.wait_for_service('moveWithVelocity')
    try:
        moveToPosition = rospy.ServiceProxy('moveWithVelocity', Velocity)
        res = moveToPosition(velocity)
        return res
    except rospy.ServiceException, e:
        print("service call failed: %s" % e)


def callback_man(data):
    print(type(data))
    print(data.data)
    data = data.data.split(",")
    print(type(data[8]))
    request_velocity(int(data[8]))


def man_res():
    rospy.init_node('Manupulator')
    sub = rospy.Subscriber('/Thrust_values', String, callback_man)


if __name__ == "__main__":
    man_res()
    rospy.spin()
