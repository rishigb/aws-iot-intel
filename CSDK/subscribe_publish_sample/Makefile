CC = gcc

#remove @ for no make command prints
DEBUG=@

APP_DIR = .
APP_NAME=subscribe_publish_sample
APP_SRC_FILES=$(APP_NAME).c

#IoT client directory
IOT_CLIENT_DIR=../../iot_src
IOT_INCLUDE_DIRS += -I $(IOT_CLIENT_DIR)/protocol/mqtt
IOT_INCLUDE_DIRS += -I $(IOT_CLIENT_DIR)/protocol/mqtt/paho_embeddedC
IOT_INCLUDE_DIRS += -I $(IOT_CLIENT_DIR)/protocol/mqtt/paho_embeddedC/platform_linux
IOT_INCLUDE_DIRS += -I $(IOT_CLIENT_DIR)/utils

PLATFORM_DIR = $(IOT_CLIENT_DIR)/protocol/mqtt/paho_embeddedC/platform_linux
IOT_SRC_FILES += $(IOT_CLIENT_DIR)/protocol/mqtt/paho_embeddedC/mqtt_pahoEmbC_wrapper.c
IOT_SRC_FILES += $(shell find $(PLATFORM_DIR)/ -name '*.c')

#MQTT Paho Embedded C client directory
MQTT_DIR = ../../mqtt_paho_emb
MQTT_C_DIR = $(MQTT_DIR)/MQTTClient-C/src
MQTT_EMB_DIR = $(MQTT_DIR)/MQTTPacket/src

MQTT_INCLUDE_DIR += -I $(MQTT_EMB_DIR)
MQTT_INCLUDE_DIR += -I $(MQTT_C_DIR)

MQTT_SRC_FILES += $(shell find $(MQTT_EMB_DIR)/ -name '*.c')
MQTT_SRC_FILES += $(MQTT_C_DIR)/MQTTClient.c


#TLS - openSSL
TLS_LIB_DIR = /usr/lib/
TLS_INCLUDE_DIR = -I /usr/include/openssl
EXTERNAL_LIBS += -L$(TLS_LIB_DIR)
LD_FLAG += -Wl,-rpath,$(TLS_LIB_DIR)
LD_FLAG += -ldl -lssl -lcrypto

#Aggregate all include and src directories
INCLUDE_ALL_DIRS += $(IOT_INCLUDE_DIRS) 
INCLUDE_ALL_DIRS += $(MQTT_INCLUDE_DIR) 
INCLUDE_ALL_DIRS += $(TLS_INCLUDE_DIR)
INCLUDE_ALL_DIRS += $(APP_INCLUDE_DIRS)
 
SRC_FILES += $(MQTT_SRC_FILES)
SRC_FILES += $(APP_SRC_FILES)
SRC_FILES += $(IOT_SRC_FILES)

# Logging level control
LOG_FLAGS += -DIOT_DEBUG
LOG_FLAGS += -DIOT_INFO
LOG_FLAGS += -DIOT_WARN
LOG_FLAGS += -DIOT_ERROR

COMPILER_FLAGS += -g
COMPILER_FLAGS += $(LOG_FLAGS)

MAKE_CMD = $(CC) $(SRC_FILES) $(COMPILER_FLAGS) -o $(APP_NAME) $(LD_FLAG) $(EXTERNAL_LIBS) $(INCLUDE_ALL_DIRS)

all:
	$(DEBUG)$(MAKE_CMD)
	
clean:
	rm -rf $(APP_DIR)/$(APP_NAME)	
