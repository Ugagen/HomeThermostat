import PyCmdMessenger

devAddress = "/dev/ttyUSB0"
arduino = PyCmdMessenger.ArduinoBoard(devAddress,baud_rate=9600)

# List of command names (and formats for their associated arguments). These must
# be in the same order as in the sketch.
commands = [["curr_temp",""],
            ["curr_temp_is","s"],
            ["default_temp",""],
            ["default_temp_is","s"],
            ["set_temp","d"],
            ["power","?"],
            ["power_is","s"],
            ["error","s"]]

# Initialize the messenger
c = PyCmdMessenger.CmdMessenger(arduino,commands)


def get_curr_temp():
    c.send("curr_temp")
    msg = c.receive()
    return msg

def get_def_temp():
    c.send("default_temp")
    msg = c.receive()
    return msg

# temperature must be float
def set_temp(temp):
    if type(temp) != float:
        msg = "temperature should be float type"
        return msg
    c.send("set_temp",temp)
    msg = c.receive()
    return msg

# power must be boolean type
def turn_power(turn):
    if type(temp) != bool:
        msg = "temperature should be float type"
        return msg
    c.send("power", turn)
    msg = c.receive()
    return msg

