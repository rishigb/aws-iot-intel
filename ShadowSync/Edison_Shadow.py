from subprocess import call
import time
import pyupm_grove as grove

#Input your thing thing here:
thing_name = "EdisonGroveLight"

# Create the light sensor object using AIO pin 0
light = grove.GroveLight(0)

# Read the input and print both the raw value and a rough lux value,
# waiting one second between readings
while 1:
    print light.name() + " raw value is %d" % light.raw_value() + \
        ", which is roughly %d" % light.value() + " lux";
    
    payload = "\"{\\\"state\\\":{\\\"reported\\\":{\\\"lux\\\": \\\"" +str(light.value())+ "\\\"}}}\""
 
    print("sending payload to AWS")
    shadow_command = "iot-data update-thing-shadow --thing-name "+ thing_name +" --payload " + payload + " output.txt"
	
    call("aws " + shadow_command, shell=True)

    time.sleep(5)

# Delete the light sensor object

del light
