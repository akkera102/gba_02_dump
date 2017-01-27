import time
import serial
import struct

COM = "COM10"


print "GBA Dumper + Arduino DUE"
st = time.clock()

# buffer clear
ser = serial.Serial(port=COM, baudrate=115200)
time.sleep(0.1)
ser.close()


# restart
ser = serial.Serial(port=COM, baudrate=115200)

s = 0
for i in xrange(4):
	s <<= 8
	s = s + ord(ser.read(1))

print "ROM SIZE: " + hex(s)


d = []
for i in xrange(s/2):
	d.append(ord(ser.read(1)))
	d.append(ord(ser.read(1)))

	if(i % 0x1000 == 0 and i != 0):
		print "received " + hex(i*2)

ser.close()


f = open("test.gba", "wb+")
for x in d:
	f.write(struct.pack("B", x))

f.close()
print "done"

ed = time.clock()
print "total " + str(int(ed - st) / 60) + " min"
