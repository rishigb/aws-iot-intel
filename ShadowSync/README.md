##Using Things Shadow


Icebreaker enables bidirectional data exchange between an internet connected device, in our case the Intel Edison, and the AWS cloud service. To accomplish this the state of the device is stored is a data store called the Thing Shadow. This means that Icebreaker will always be able to read the last recorded value regardless of the connectivity of your device. The Shadow data structure can also be used to interact with your device remotely by updating the desired state of the device. 

###Simulated Device


First we will use the Shadow as a device simulator by updating the state of our device manually.
Create a Json file on the Edison that contains a simulated state of a device. Lets assume that the device in question will be a light sensor connected to the Edison. We will send a JSON payload to the thing shadow telling AWS that our device picked up a lux (SI unit of luminosity) of 8.  

```json
{
"state": {
"reported": {
"lux":"8"}
}
}
```

To create a thing and update its state use the following AWS cli command:

```
aws iot-data --endpoint-url https://g.us-east-1.pb.iot.amazonaws.com update-thing-shadow --thing-name "EdisonGroveLight" --payload "{ \"state\": { \"reported\": { \"lux\": \"8\" } } }" output.txt
```

To read the state of the thing on the Shadow data store use the following AWS cli command:

```
aws iot-data --endpoint-url https://g-ws.us-east-1.pb.iot.amazonaws.com get-thing-shadow --thing-name EdisonGroveLight received_state.txt
```

The Aws model “iot-data” is only responsible for sending and retrieving thing states. In order to modify your thing the aws model “iot” is used. 
For instance to list all of the Things in your AWS account you would use:

```
aws iot --endpoint-url https://t71u6yob51.execute-api.us-east-1.amazonaws.com/beta list-things
```

To see the full list of commands enter:

```
aws iot help
```

Likewise to see detailed help on a command, update-thing for instance, use:

```
aws iot update-thing help
```

###Connect actual device


Now lets connect an actual Grove light sensor to the Intel Edison and send real time data up to AWS.
Plug in a grove senor shield into the Edison and attach a light sensor to A0

![Grove Light Sensor connected to a0](http://i.imgur.com/GwB8Mrm.png)

Once you have hooked up the sensor as shown above upload the sample application Edison_Shadow.py to your Edison. 
The easiest way to do this is with FileZila or a similar FTP program. 


Once the script is uploaded run it with:

```
python Edison_Shadow.py
```

The script takes a light level reading every 5 seconds and send a Thing Shadow state update with that value to AWS. 


To see your device state changes log into AWS IoT console and select EdisonGroveLight:

![icebreaker console](https://cloud.githubusercontent.com/assets/4919014/10322265/d7439d04-6c31-11e5-9528-1df42be0c409.png)

