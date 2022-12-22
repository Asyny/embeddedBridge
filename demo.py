import time
import serial

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=115200,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=5,
    write_timeout=5
)

ser.isOpen()


commandList = []
commandList.append("RELais:A:ACTivate")
commandList.append("RELais:A?")
commandList.append("RELais:A:DEACTivate")
commandList.append("RELais:A?")
commandList.append("RELais:B:ACTivate")
commandList.append("RELais:B?")
commandList.append("RELais:B:DEACTivate")
commandList.append("RELais:B?")
commandList.append("Wrong:COmmand?")

for command in commandList:
    ser.write(bytes(command + '\r',"utf-8"))

    value = ""
    if '?' in command:
        timeout = time.time() + 5
        while ser.inWaiting() == 0 and time.time() < timeout:
            time.sleep(0.01)
        value = ser.readline().decode("utf-8")
        value = value.replace("\n", "")
        value = value.replace("\r", "")
        print("\"" + command + "\"" + " returns " + value)

    timeout = time.time() + 5
    while ser.inWaiting() == 0 and time.time() < timeout:
        time.sleep(0.01)
    returnCode = ser.readline().decode("utf-8")

    returnCode = returnCode.replace("\n", "")
    returnCode = returnCode.replace("\r", "")
    print("\"" + command + "\"" + " with status " + returnCode)

print("############## finished #########")
