#pragma once

#include <string>

namespace utils {

class UniqueIDGenerator {
 public:
  UniqueIDGenerator() {
    // Set the machine ID, for example, a hashed version of the MAC address.
    machine_id = 12345; // Replace this with a real hashed machine ID.
    sequence_number = 0;
  }

  uint64_t generate_id() {
    uint64_t timestamp = get_timestamp();
    uint64_t id = (timestamp << 22) | (machine_id << 12) | (sequence_number & 0xFFF);
    sequence_number = (sequence_number + 1) & 0xFFF;
    return id;
  }

 private:
  uint16_t machine_id;
  uint16_t sequence_number;

  uint64_t get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return static_cast<uint64_t>(milliseconds);
  }
};

}  // namespace utils
