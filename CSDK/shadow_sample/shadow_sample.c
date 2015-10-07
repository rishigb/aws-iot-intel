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
#include "iot_shadow.h"
#include "json_utils.h"
#include "iot_log.h"

/*
{
  "reported": {
    "temperature":0.0,
    "windowOpen":false
  },
  "desired": {
    "windowOpen":false,
  }
}
*/

typedef struct {
  double temperature;
  bool windowOpen;
} ShadowReported;

ShadowReported reported;

typedef struct {
  bool windowOpen;
} ShadowDesired;

ShadowDesired desired;

char shadowTxBuffer[256];
char deltaBuffer[256];

static jsmn_parser json_parser;
static jsmntok_t tokens[20]; // max expected tokens is 20

void ShadowCallbackHandler(void) {
	IoT_Error_t rc = NONE_ERROR;
	int ret;

	INFO("My State Changed");
	INFO("delta:%s", deltaBuffer);

	// init json parser
	jsmn_init(&json_parser);

	ret = jsmn_parse(&json_parser, deltaBuffer, strlen(deltaBuffer), tokens, sizeof(tokens) / sizeof(tokens[0]));

	if (ret < 0) {
		WARN("Failed to parse JSON: %d", ret);
	}

	// Assume the top-level element is a JSON object
	if (ret < 1 || tokens[0].type != JSMN_OBJECT) {
		WARN("Error: object expected");
		return;
	}

	int version = 0;
	int tIndex = 0;

	// JSON --> struct
	for (tIndex = 1; tIndex < ret; tIndex++) {
		if (jsoneq(deltaBuffer, &tokens[tIndex], "window_open") == 0) {
			rc = parseBooleanValue(&desired.windowOpen, deltaBuffer, tokens+tIndex+1);
			INFO("Now %s window.", desired.windowOpen ? "opening" : "closing");
			// for now just make "reported" mirror desired
			reported.windowOpen = desired.windowOpen;
		}
	}
}

char certDirectory[PATH_MAX + 1] = "../../tls/certs";
char HostAddress[255] = "g.us-east-1.pb.iot.amazonaws.com";
int port = 8883;
int numPubs = 5;

void parseInputArgsForConnectParams(int argc, char** argv) {
	int opt;

	while (-1 != (opt = getopt(argc, argv, "h:p:c:n:"))) {
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
		case 'n':
			numPubs = atoi(optarg);
			DEBUG("num pubs %s", optarg);
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
			ERROR("ERROR in command line argument parsing");
			break;
		}
	}

}

int main(int argc, char** argv) {
	IoT_Error_t rc = NONE_ERROR;
	int i = 0;

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

	DEBUG("Using rootCA %s", rootCA);
	DEBUG("Using clientCRT %s", clientCRT);
	DEBUG("Using clientKey %s", clientKey);

	ShadowParameters sp;
	sp.pThingId = "csdkshadow";
	sp.pHost = HostAddress;
	sp.port = port;
	sp.pClientCRT = clientCRT;
	sp.pClientKey = clientKey;
	sp.pRootCA = rootCA;
	sp.stateChangeHandler = ShadowCallbackHandler;
	sp.deltaBuffer = deltaBuffer;

	INFO("Shadow Init");
	rc = iot_shadow_init(&sp);

	INFO("Shadow Connect");
	rc = iot_shadow_connect();

	if (NONE_ERROR != rc) {
		ERROR("Shadow Connection Error");
	}

	// reported values
	reported.temperature = 75.5;
	reported.windowOpen = false;

	uint8_t loopcnt = 0;
	uint16_t pubcnt = 0;
	uint8_t tempSensorIndex = 0;
	double tempSensor[] = {75.4, 70.1, 68.7, 71.2, 73.1};

	// loop and publish a change in temperature
	while (NONE_ERROR == rc && pubcnt < numPubs) {

		rc = iot_shadow_yield(100);
		sleep(1);

		// publish every 5 seconds
		if (loopcnt == 4) {
			loopcnt = 0;

			// update temperature "sensor" value
			reported.temperature = tempSensor[tempSensorIndex];
			// update index into "sensor" array - loop if > 5 pubs
			tempSensorIndex = tempSensorIndex >= 4 ? 0 : tempSensorIndex+1;
			INFO("Current Temperature: %.1lf", reported.temperature);

			// struct --> JSON
			sprintf(shadowTxBuffer, "{\"temperature\":\"%.1lf\",\"window_open\":%s}",
					reported.temperature, (reported.windowOpen ? "true" : "false"));

			rc = iot_shadow_sync_reported(shadowTxBuffer);
			pubcnt++;
		} else {
			loopcnt++;
		}
	}

	if (NONE_ERROR != rc) {
		ERROR("An error occurred in the loop.");
	}

	INFO("Disconnecting");
	rc = iot_shadow_disconnect();

	if (NONE_ERROR != rc) {
		ERROR("Disconnect error");
	}

	return 0;
}
