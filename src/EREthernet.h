#ifndef ER_ETHERNET_H
#define ER_ETHERNET_H
#include <Ethernet.h>
#include <PubSubClient.h>

class EREthernet {
public:
    /** Constructor */
    EREthernet(byte* macAddress, IPAddress ipAddress, IPAddress server);

    typedef void (*Callback)(byte *payload, unsigned int length);

    typedef struct {
        char* command;
        Callback callback;
    } Command;

    /** Initialize the Ethernet connection */
    void begin();

    /** Check the status of the Ethernet connection and process messages */
    void loop();

    /** Set the commands that the Ethernet client will respond to */
    void setCommands(Command* commands, int numCommands);

    /** Send a message via MQTT */
    void send(const char* payload);

    /** Set the client ID for MQTT connection */
    EREthernet& setClientId(const char* clientId);

    /** Set the topic to subscribe to for MQTT messages */
    EREthernet& setTopicSub(const char* topic_sub);
    
    /** Set the topic to publish to for MQTT messages */
    EREthernet& setTopicPub(const char* topic_pub);

    /** Set the connection string for MQTT connection */
    EREthernet& setConnectionStr(const char* connection_str);

private:
    byte* mac;
    IPAddress ip;
    IPAddress server;

    long lastReconnectAttempt = 0;

    char* clientId = nullptr;
    char* topic_sub = nullptr;
    char* topic_pub = nullptr;
    char* connection_str = nullptr;

    EthernetClient ethClient;
    PubSubClient client;

    Command* commands;
    int numCommands;

    /** Callback function for handling incoming messages */
    void callback(char *topic, byte *payload, unsigned int length);

    /** Static trampoline to forward C-style callback to instance method */
    static void staticCallback(char *topic, byte *payload, unsigned int length);

    /** Pointer to the current instance used by the static trampoline */
    static EREthernet* instance;

    /** Attempt to reconnect to the MQTT server */
    bool reconnect();
};

#endif // ER_ETHERNET_H
