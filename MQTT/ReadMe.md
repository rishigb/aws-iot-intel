
## Use MQTT to subscribe and publish to AWS IoT

Now that the certificates are in order we can use MQTT to subscribe and publish to the cloud. 
First get the root CA pem file:
``` bash
curl https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem > rootCA.pem 
``` 
Use the moquitto client to subscribe to a topic:
``` bash
$ mosquitto_sub --cafile rootCA.pem --cert certs\cert.pem --
key privateKey.pem -h g.us-east-1.pb.iot.amazonaws.com -p 8883
-q 1 -d -t <topic> -i <client_id>
``` 
**NOTE:** replace <topic> and <client_id> with the topic you wish to subscribe to and the id you wish to have. 
Use mosquitto to publish to a topic: 
``` bash
$ mosquitto_pub --cafile crootCA.pem --cert certs\cert.pem --
key privateKey.pem -h g.us-east-1.pb.iot.amazonaws.com -p 8883
-q 1 -d -t <topic> -i <client_id> -m <"message">
``` 
NOTE: Replace <topic>, <client_id>, and <"message"> with the topic, client id and message you wish to publish. the message must be in quotes.  


 
