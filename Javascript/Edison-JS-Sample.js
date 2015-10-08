/*
 * 
 * Copyright (c) 2015 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

//NOTE: This code is unfinished and the dependencies are incomplete. It should be used for reference only at this point  

//node.js deps

//npm deps

//app deps
const thingShadow = require('./thing');
//const isUndefined = require('../common/lib/is-undefined');
//const cmdLineProcess   = require('./lib/cmdline');

//UPM dependencies 
// Load Grove module
var groveSensor = require('jsupm_grove');



// Create the light sensor object using AIO pin 0
var light = new groveSensor.GroveLight(0);

// Create the temperature sensor object using AIO pin 1
var temp = new groveSensor.GroveTemp(1);

//Define your device name
var Device_Name = 'Device Name';


//define AWS certificate path paramenters
var args = {


	privateKey:'/home/root/aws_certs/privateKey.pem',
	clientCert:'/home/root/aws_certs/cert.pem',
	caCert:'/home/root/aws_certs/aws-iot-rootCA.crt',
	clientId:'icebreaker_edison',
	region:'us-east-1', //at time of writing only region that supports AWS IoT
	reconnectPeriod:'10' //asumming reconnect period in seconds
} 


//create global state variable

var reported_state={ lux: 0, temp: 0};

//create global sensor value variables:

var read_lux = 0;
var read_temp = 0;

//launch sample app function 
update_state(args);

function update_state(args) {

//create a things Shadows object

const thingShadows = thingShadow({
  keyPath: args.privateKey,
  certPath: args.clientCert,
  caPath: args.caCert,
  clientId: args.clientId,
  region: args.region,
  reconnectPeriod: args.reconnectPeriod,
});

//When Thing Shadows connects to AWS server:


thingShadows
  .on('connect', function() {
  	console.log('registering device: '+ Device_Name)

  	//register device
  	thingShadows.register(Device_Name);

  	
  	//read sensor values and send to AWS IoT every 5 seconds 
  	setInterval(function(){

  	read_sensor(send_state); 

	}, 5000);

  });


// motitor for events in the stream and print to console:

thingShadows 
  .on('close', function() {
    console.log('close');
  });
thingShadows 
  .on('reconnect', function() {
    console.log('reconnect');
  });
thingShadows 
  .on('offline', function() {
    console.log('offline');
  });
thingShadows
  .on('error', function(error) {
    console.log('error', error);
  });
thingShadows
  .on('message', function(topic, payload) {
    console.log('message', topic, payload.toString());
  });

};

//define function for reading sensor
  function read_sensor(cb){

  	read_lux = light.value();
  	read_temp = temp.value();

  	cb();
  };

//define function for updating thing state:

  function send_state(){

  	//define the payload with sensor values
  	reported_state ={ lux: read_lux, temp: read_temp};

  	//create state update payload JSON:
  	device_state={state: { reported: reported_state }};

  	//send update payload to aws:
  	thingShadows.update(Device_Name, device_state );

  };
