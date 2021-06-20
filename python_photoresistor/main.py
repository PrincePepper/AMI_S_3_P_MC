import dweepy
import serial
import serial.tools.list_ports as prtlst

global COMs
COMs = []
data = [0] * 60


def getCOMs():
    global COMs
    pts = prtlst.comports()

    for pt in pts:
        if pt.serial_number is not None:
            if "754373037303514192E1" in pt.serial_number:
                COMs.append(pt[0])


getCOMs()
ser = serial.Serial(COMs[0])
pos = 0
while 1:
    light_data = int(ser.readline())
    data[pos] = light_data
    if pos == len(data) - 1:
        pos = 0
        temp_data = sorted(data)
        dweepy.dweet_for('photoresistor', {'Median light per minute': temp_data[int(len(temp_data) / 2)]})
    else:
        pos += 1
    dweepy.dweet_for('photoresistor', {'light': light_data})
	