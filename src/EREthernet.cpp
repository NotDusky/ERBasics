#include "EREthernet.h"

EREthernet::EREthernet(byte* macAddress, IPAddress ipAddress, IPAddress server) :
    mac(macAddress), ip(ipAddress), server(server), commands(nullptr), numCommands(0), client(ethClient) {}

// Static instance pointer used by the static callback trampoline
EREthernet* EREthernet::instance = nullptr;

void EREthernet::begin() {
    if (clientId == nullptr) {
        Serial.println("Client ID not set, unable to start Ethernet connection");
        return;
    }
    if (topic_sub == nullptr) {
        Serial.println("Subscription topic not set, unable to start Ethernet connection");
        return;
    }
    if (topic_pub == nullptr) {
        Serial.println("Publication topic not set, unable to start Ethernet connection");
        return;
    }
    if (connection_str == nullptr) {
        Serial.println("Connection string not set, unable to start Ethernet connection");
        return;
    }

    client.setServer(server, 1883);
    // register static callback trampoline and set instance pointer
    EREthernet::instance = this;
    client.setCallback(EREthernet::staticCallback);

    Ethernet.begin(mac, ip);
    // Allow the hardware to sort itself out
    delay(1500);
    Serial.println(Ethernet.localIP());
}

void EREthernet::loop() {
    if (!client.connected()) {
        long now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            // Attempt to reconnect
            if (reconnect()) {
                lastReconnectAttempt = 0;
            }
        }
    } else {
        client.loop();
    }
}

void EREthernet::setCommands(Command* commands, int numCommands) {
    this->commands = commands;
    this->numCommands = numCommands;
}

void EREthernet::send(const char* payload) {
    if (client.connected()) {
        client.publish(topic_pub, payload);
    }
    #ifdef ERB_LOGGING
    else {
        Serial.println("Unable to send message, not connected to MQTT server");
    }
    #endif // ERB_LOGGING
}

EREthernet& EREthernet::setClientId(const char* clientId) {
    this->clientId = (char*)malloc(strlen(clientId) + 1);
    strcpy(this->clientId, clientId);
    return *this;
}

EREthernet& EREthernet::setTopicSub(const char* topic_sub) {
    this->topic_sub = (char*)malloc(strlen(topic_sub) + 1);
    strcpy(this->topic_sub, topic_sub);
    return *this;
}

EREthernet& EREthernet::setTopicPub(const char* topic_pub) {
    this->topic_pub = (char*)malloc(strlen(topic_pub) + 1);
    strcpy(this->topic_pub, topic_pub);
    return *this;
}

EREthernet& EREthernet::setConnectionStr(const char* connection_str) {
    this->connection_str = (char*)malloc(strlen(connection_str) + 1);
    strcpy(this->connection_str, connection_str);
    return *this;
}

// Static trampoline that forwards to the instance method
void EREthernet::staticCallback(char *topic, byte *payload, unsigned int length) {
    if (EREthernet::instance) {
        EREthernet::instance->callback(topic, payload, length);
    }
}

void EREthernet::callback(char *topic, byte *payload, unsigned int length) {
    #ifdef ERB_LOGGING
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
    #endif // ERB_LOGGING

    // Functionality
    for (int i = 0; i < numCommands; i++) {
        if (strcmp(topic, commands[i].command) == 0) {
            commands[i].callback(payload, length);
            break;
        }
    }
}

bool EREthernet::reconnect() {
    Serial.println("Attempting network reconnect");
    if (client.connect(clientId)) {
        Serial.println("Network reconnected");
        if (topic_sub != nullptr) {
            client.subscribe(topic_sub);
        }
    }
    return client.connected();
}
