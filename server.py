import serial

ser0 = serial.Serial('/dev/ttyACM0',9600, timeout=1)
ser1 = serial.Serial('/dev/ttyACM1',9600, timeout=1)
ser0.flush()
ser1.flush()

line0 = ""
line1 = ""
done = 0

while done<2:
        if ser0.in_waiting > 0:
                line0 = ser0.readline().decode('utf-8').rstrip()
                print("ACM0 output:")
                print(line0)
                done = done + 1
        if ser1.in_waiting > 0:
                line1 = ser1.readline().decode('utf-8').rstrip()
                print("ACM1 output:")
                print(line1)
                done = done + 1


while True:
        if int(line0)<int(line1):
                ser0.write("1\n".encode())
                ser1.write("0\n".encode())
                print("ACM0 is winner")
        elif int(line0)>int(line1):
                ser0.write("0\n".encode())
                ser1.write("1\n".encode())
                print("ACM1 is winner")
        else:
                ser0.write("9\n".encode())
                ser1.write("9\n".encode())
                print("tie")