#include <WiFi.h>
#include <MQTTPubSubClient.h>
#include <PubSubClient.h>provides a set of functions and classes that make it easier to interact with MQTT brokers, publish and subscribe to topics, and manage MQTT connections 
 

#define TOKEN "BBFF-Nx4xHjEsmkvTnEaen8x5G67ZRJSYgE"
#define MQTT_CLIENT_NAME "mymqttclientname"
#define VARIABLE_LABEL1 "MQ135"
#define VARIABLE_LABEL2 "MQ3"
#define VARIABLE_LABEL3 "MQ2"
#define DEVICE_LABEL "esp32"
#define SENSOR1 34
#define SENSOR2 35
#define SENSOR3 32
char WIFISSID[] ="Jenishalebana";   
//"Jone";
char PASSWORD[] ="123456789";
//"jone2727";

const char* mqttBroker = "industrial.api.ubidots.com";//This part declares a constant character pointer....mqttBroker to refer to the memory location that holds the string.
char payload[100];
char topic[150];
char str_sensor[10];

WiFiClient ubidots;//This line declares an instance of the WiFiClient class named ubidots. The WiFiClient class is often used in Arduino or ESP8266/ESP32 projects for creating a client that can communicate over Wi-Fi networks. In this case, it appears that you are creating a Wi-Fi client instance to connect to a network.
PubSubClient client(ubidots);//This line declares an instance of the PubSubClient class named client. The PubSubClient class is often used for working with MQTT (Message Queuing Telemetry Transport) communication in IoT applications.

void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];//// Create a character array 'p' with the same length as the payload and add a null terminator.
    memcpy(p, payload, length);
    p[length] = '\0';
    String message(p);// Convert the character array 'p' into a String object 'message'.
    Serial.write(payload, length);// Write the payload data to the Serial output.
    Serial.println(topic);// Print the MQTT topic to the Serial output.
}

void reconnect() {
    while (!client.connected()) {
      // Loop until the client is connected.
        Serial.println("Attempting MQTT connection...");// Attempt to connect to the MQTT broker.
        if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
            Serial.println("Connected");
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 2 seconds");// If the connection attempt fails, print an error message and wait for 2 seconds before trying again.
            delay(2000);
        }
    }
}

void setup() {
    Serial.begin(115200);// Initialize serial communication for debugging

    pinMode(SENSOR1, INPUT);// Set the mode of a pin named SENSOR1 as INPUT.

    WiFi.begin(WIFISSID, PASSWORD);// Connect to the Wi-Fi network.
    Serial.println(); 
    Serial.print("Wait for WiFi...");// Wait until the device is successfully connected to Wi-Fi.
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");// Print a message indicating that Wi-Fi is connected.
    Serial.println("WiFi Connected");
    Serial.println("IP address: "); 
    Serial.println(WiFi.localIP());

    client.setServer(mqttBroker, 1883);// Set up the MQTT client by specifying the MQTT broker and callback function.
    client.setCallback(callback);
}

void loop() {
  // Check if the MQTT client is not connected, and if not, attempt to reconnect.
    if (!client.connected()) {
        reconnect();
    }
    sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);// Construct the MQTT topic.
    sprintf(payload, "%s", "");// Initialize the payload string.
    sprintf(payload, "{\"%s\":", VARIABLE_LABEL1); // Add the initial part of the payload JSON.
    float mq135 = analogRead(SENSOR1);// Read sensor data from analog pins.
    float mq3 = analogRead(SENSOR2);
    float mq2 = analogRead(SENSOR3); 
    dtostrf(mq135, 4, 2, str_sensor);// Convert sensor data to strings.
    sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor);// Append the sensor data to the payload JSON.
    Serial.println("Publishing data to Ubidots Cloud"); // Print information to the Serial Monitor
    Serial.println(payload);
    client.publish(topic, payload); // Publish the payload to the MQTT broker.
    client.loop();// Keep the MQTT client's connection alive.
    delay(1000);
}
