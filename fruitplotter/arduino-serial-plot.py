#!/usr/bin/python3

import serial
import time
import sys
import matplotlib.pyplot as plt
import numpy as np

if len(sys.argv) == 3:
    ser = serial.Serial(sys.argv[1], sys.argv[2])
else:
    ser = serial.Serial("/dev/ttyACM0", 115200)

plt.axis([0, 100, 5, -100])
plt.ion()
plt.show()

xdata = [ ]
ydata = [ [], [], [], [], [] ]
lines = [ ]

for i in range(5):
    line, = plt.plot(xdata, ydata[i])
    lines.append(line)

plt.legend(lines, ["Center", "LL", "UL", "UR", "LR"])

ts_start = time.time()
data_being_sent = False

def add_point(points: list, value: int):
    if len(points) >= 100:
        points.pop(0)

    points.append(value)


def read_byte():
    byties = ser.read(size=1)
    value = int.from_bytes(byties, byteorder='little', signed=True)
    return value


while 1:

    if data_being_sent:
        value = read_byte()
        if value == 127:
            p_x = time.time() - ts_start
            add_point(xdata, p_x)
            print("Got 127")
            for i in range(5):
                value = read_byte()
                print("F{}: {} ".format(i, value), end='')
                add_point(ydata[i], value)
                lines[i].set_xdata(xdata)
                lines[i].set_ydata(ydata[i])

                print(end='\n')

        plt.xlim(xdata[0], xdata[-1])

    else:

        byties = ser.readline()
        stringies = byties.decode("cp437")
        if stringies.startswith("BEGIN_FRUIT_DATA"):
            print("DATA_BEING_SENT!")
            ser.flush()
            data_being_sent = True

        sys.stdout.write(stringies)
        sys.stdout.flush()

    plt.pause(0.05)
