#include "EROneWire.h"

OneWireRFID::OneWireRFID(uint8_t pin) : reader(pin), last_read_time(0), read_interval(100), current_id(0) {};

OneWireRFID::OneWireRFID(uint8_t pin, uint32_t read_interval) : reader(pin), last_read_time(0), read_interval(read_interval), current_id(0) {};

bool OneWireRFID::update() {
    uint32_t current_time = millis();
    if (current_time - last_read_time >= read_interval) {
        last_read_time = current_time;
        uint32_t card_id = check_for_card();
        if (card_id != current_id) {
            current_id = card_id;
            return true;
        }
    }
    return false;
};

uint32_t OneWireRFID::check_for_card() {
    static byte addr[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    reader.reset_search();
    if (!reader.search(addr)) return 0;
    
    // Cast address to a pair of 32 bit unsigned integers
    uint32_t *addr_32 = (uint32_t*)addr;
    // Return first 4 bytes (32 bits) of address
    return addr_32[0];
};

uint32_t OneWireRFID::force_read(bool restart_timer) {
    uint32_t card_id = check_for_card();
    if (card_id != current_id) {
        current_id = card_id;
        if (restart_timer) last_read_time = millis();
    }
    return current_id;
};
