#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include <signal.h>
#include <memory.h>
#include <sys/time.h>
#include <limits.h>
#include "mqtt_interface.h"
#include "iot_version.h"
#include "iot_log.h"

int MQTTcallbackHandler(MQTTCallbackParams params) {

	INFO("Subscribe callback");
	INFO("%.*s\t%.*s",
			(int)params.TopicNameLen, params.pTopicName,
			(int)params.MessageParams.PayloadLen, (char*)params.MessageParams.pPayload);

	return 0;
}

uint8_t qos = 0;

char certDirectory[PATH_MAX + 1] = "../../tls/certs";
char HostAddress[255] = "g.us-east-1.pb.iot.amazonaws.com";
uint16_t port = 8883;

void parseInputArgsForConnectParams(int argc, char** argv) {
	int opt;

	while (-1 != (opt = getopt(argc, argv, "h:p:c:"))) {
		switch (opt) {
		case 'h':
			strcpy(HostAddress, optarg);
			DEBUG("Host %s", optarg);
			break;
		case 'p':
			port = atoi(optarg);
			DEBUG("arg %s", optarg);
			break;
		case 'c':
			strcpy(certDirectory, optarg);
			DEBUG("cert root directory %s", optarg);
			break;
		case '?':
			if (optopt == 'c') {
				ERROR("Option -%c requires an argument.", optopt);
			}
			else if (isprint(optopt)) {
				WARN("Unknown option `-%c'.", optopt);
			}
			else {
				WARN("Unknown option character `\\x%x'.", optopt);
			}
			break;
		default:
			ERROR("Error in command line argument parsing");
			break;
		}
	}

}

int main(int argc, char** argv) {
	IoT_Error_t rc = NONE_ERROR;
	int32_t i = 0;

	char rootCA[PATH_MAX + 1];
	char clientCRT[PATH_MAX + 1];
	char clientKey[PATH_MAX + 1];
	char CurrentWD[PATH_MAX + 1];
	char cafileName[] = "/aws-iot-rootCA.crt";
	char clientCRTName[] = "/cert.pem";
	char clientKeyName[] = "/privkey.pem";

	parseInputArgsForConnectParams(argc, argv);


	INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);


	getcwd(CurrentWD, sizeof(CurrentWD));
	sprintf(rootCA, "%s/%s%s", CurrentWD, certDirectory, cafileName);
	sprintf(clientCRT, "%s/%s%s", CurrentWD, certDirectory, clientCRTName);
	sprintf(clientKey, "%s/%s%s", CurrentWD, certDirectory, clientKeyName);

	DEBUG("rootCA %s", rootCA);
	DEBUG("clientCRT %s", clientCRT);
	DEBUG("clientKey %s", clientKey);

	MQTTConnectParams connectParams;

	connectParams.KeepAliveInterval_sec = 10;
	connectParams.isCleansession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = "CSDK-test-device";
	connectParams.pHostURL = HostAddress;
	connectParams.port = port;
	connectParams.isWillMsgPresent = false;
	connectParams.pUserName = NULL;
	connectParams.pPassword = NULL;
	connectParams.pRootCALocation = rootCA;
	connectParams.pDeviceCertLocation = clientCRT;
	connectParams.pDevicePrivateKeyLocation = clientKey;
	connectParams.commandTimeout_ms = 10000;
	connectParams.isSSLHostnameVerify = true;// ensure this is set to true for production

	INFO("Connecting...");
	rc = iot_mqtt_connect(&connectParams);
	if (NONE_ERROR != rc) {
		ERROR("Error[%d] connecting to %s:%d", rc, connectParams.pHostURL, connectParams.port);
	}

	MQTTSubscribeParams subParams;
	subParams.mHandler = MQTTcallbackHandler;
	subParams.pTopic = "sdkTest/sub";
	subParams.qos = qos;

	if (NONE_ERROR == rc) {
		INFO("Subscribing...");
		rc = iot_mqtt_subscribe(&subParams);
		if (NONE_ERROR != rc) {
			ERROR("Error subscribing");
		}
	}

	MQTTMessageParams Msg;
	Msg.qos = QOS_0;
	Msg.isRetained = false;
	char cPayload[100];
	sprintf(cPayload, "%s : %d ", "hello from SDK", i);
	Msg.pPayload = (void *) cPayload;
	Msg.PayloadLen = strlen(cPayload) + 1;

	MQTTPublishParams Params;
	Params.pTopic = "sdkTest/sub";
	Params.MessageParams = Msg;

	while (NONE_ERROR == rc) {
		rc = iot_mqtt_yield(100);
		INFO("-->sleep");
		sleep(1);
		sprintf(cPayload, "%s : %d ", "hello from SDK", i++);
		rc = iot_mqtt_publish(&Params);
	}

	ERROR("An error occurred in the loop.");

	return rc;
}

