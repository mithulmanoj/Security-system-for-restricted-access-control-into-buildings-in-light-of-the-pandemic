/*
 * mqtt_client_config.c
 *
 *  Created on: 11-Aug-2021
 *      Author: mithu
 */

#include <stdio.h>
#include "mqtt_client_config.h"

/******************************************************************************
* Global Variables
*******************************************************************************/
/* Configure the MQTT Broker/Server details. */
struct IotNetworkServerInfo networkServerInfo =
{
    .pHostName = MQTT_BROKER_ADDRESS,
    .port = MQTT_PORT
};

#if (MQTT_SECURE_CONNECTION)
/* Configure the MQTT client credentials in case of a secure connection. */
struct IotNetworkCredentials credentials =
{
    /* Configure the client certificate. */
    .pClientCert = (const char *)CLIENT_CERTIFICATE,
    .clientCertSize = sizeof(CLIENT_CERTIFICATE),
    /* Configure the client private key. */
    .pPrivateKey = (const char *)CLIENT_PRIVATE_KEY,
    .privateKeySize = sizeof(CLIENT_PRIVATE_KEY),
    /* Configure the Root CA certificate of the MQTT Broker/Server. */
    .pRootCa = (const char *)ROOT_CA_CERTIFICATE,
    .rootCaSize = sizeof(ROOT_CA_CERTIFICATE),
    /* Application Layer Protocol Negotiation (ALPN) is used to implement
     * MQTT with TLS Client Authentication on Port 443 from client devices.
     */
    #if (MQTT_PORT == 443)
    .pAlpnProtos = MQTT_ALPN_PROTOCOL_NAME
    #else
    .pAlpnProtos = NULL
    #endif
};

/* Structure with the network interface details. */
IotMqttNetworkInfo_t networkInfo =
{
    .createNetworkConnection = true,
    .u.setup.pNetworkCredentialInfo = &credentials,
    .u.setup.pNetworkServerInfo = &networkServerInfo
};

#else
/* Structure with the network interface details. */
IotMqttNetworkInfo_t networkInfo =
{
    .createNetworkConnection = true,
    /* Set the credentials to NULL for a non-secure connection. */
    .u.setup.pNetworkCredentialInfo = NULL,
    .u.setup.pNetworkServerInfo = &networkServerInfo
};
#endif /* #if (MQTT_SECURE_CONNECTION) */

/* Last Will and Testament (LWT) message structure. The MQTT broker will
 * publish the LWT message if this client disconnects unexpectedly.
 */
IotMqttPublishInfo_t willInfo =
{
    .qos = IOT_MQTT_QOS_0,
    .pTopicName = MQTT_WILL_TOPIC_NAME,
    .topicNameLength = (uint16_t)(sizeof(MQTT_WILL_TOPIC_NAME) - 1),
    .pPayload = MQTT_WILL_MESSAGE,
    .payloadLength = (size_t)(sizeof(MQTT_WILL_MESSAGE) - 1)
};

/* MQTT connection information structure. */
IotMqttConnectInfo_t connectionInfo =
{
    .cleanSession = true,
    .awsIotMqttMode = AWS_IOT_MQTT_MODE,
    .keepAliveSeconds = MQTT_KEEP_ALIVE_SECONDS,
    .pWillInfo = &willInfo,
    .pUserName = NULL,
    .pPassword = NULL,
    .userNameLength = 0,
    .passwordLength = 0
};

/* Check for a valid QoS setting.
 * The MQTT library currently supports only QoS 0 and QoS 1.
 */
#if ((MQTT_MESSAGES_QOS != 0) && (MQTT_MESSAGES_QOS != 1))
    #error "Invalid QoS setting! MQTT_MESSAGES_QOS must be either 0 or 1."
#endif

/* Check if the macros are correctly configured for AWS IoT Broker. */
#if (!MQTT_SECURE_CONNECTION && AWS_IOT_MQTT_MODE)
    #error "AWS IoT does not support unsecured connections!"
#endif

/* [] END OF FILE */

