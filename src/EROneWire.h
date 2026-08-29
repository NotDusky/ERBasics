#ifndef ER_ONE_WIRE_H
#define ER_ONE_WIRE_H

#include <Arduino.h>
#include <OneWire.h>

class OneWireRFID {
public:
    /**
     * Constructor for the OneWireRFID class
     * @param pin The pin number to which the OneWire RFID reader is connected
     */
    OneWireRFID(uint8_t pin);

    /**
     * Constructor for the OneWireRFID class
     * @param pin The pin number to which the OneWire RFID reader is connected
     * @param read_interval The interval at which to read the RFID reader
     */
    OneWireRFID(uint8_t pin, uint32_t read_interval);

    /**
     * Set the interval at which to read the RFID reader
     * @param interval The interval in milliseconds
     */
    inline void set_read_interval(uint32_t interval) {
        read_interval = interval;
    }

    /**
     * Update the RFID reader state
     * @return true if the card ID has changed since the last update, false otherwise
     */
    bool update();

    /**
     * Forces the reader to check for a card
     * @param restart_timer if true, reset the reader's read interval
     */
    uint32_t force_read(bool restart_timer = true);

    /**
     * Get the current RFID card ID 
     * @return the current RFID card ID
     */
    inline uint32_t get_current_id() {
        return current_id;
    };

    /**
     * Check if an RFID card is present
     * @return true if a card is present, false otherwise
     */
    inline bool is_card_present() {
        return current_id != 0;
    };

    /**
     * Check if the current RFID card's id matches the given id
     * @param id the id to compare to the current card
     * @return true if the ids match, false otherwise
     */
    inline bool matches(uint32_t id) {
        return current_id == id;
    }

private:
    OneWire reader;
    uint32_t last_read_time;
    uint32_t read_interval;
    uint32_t current_id;

    /**
     * Check for an RFID card and return its ID
     * @return the RFID card ID if present, 0 otherwise
     */
    uint32_t check_for_card();
};

#endif // ER_ONE_WIRE_H