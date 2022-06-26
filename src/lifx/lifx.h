#include "esphome.h"
#include <WiFi.h>
#include "esphome/components/network/util.h"

IPAddress notification_ip(10, 200, 0, 16);
WiFiUDP Udp;

// The LIFX Header structure
#pragma pack(push, 1)
typedef struct {
  /* frame */
  uint16_t size;
  uint16_t protocol:12;
  uint8_t  addressable:1;
  uint8_t  tagged:1;
  uint8_t  origin:2;
  uint32_t source;
  /* frame address */
  uint8_t  target[8];
  uint8_t  reserved[6];
  uint8_t  res_required:1;
  uint8_t  ack_required:1;
  uint8_t  :6;
  uint8_t  sequence;
  /* protocol header */
  uint64_t :64;
  uint16_t type;
  uint16_t :16;
  /* variable length payload follows */
} lifx_header;
#pragma pack(pop)

// Device::SetColor Payload
#pragma pack(push, 1)
typedef struct {
  uint16_t level;
} lifx_payload_device_set_power;
#pragma pack(pop)


// see SetColor https://lan.developer.lifx.com/docs/light-messages
#pragma pack(push, 1)
typedef struct {
  uint8_t reserved;
  //HSBK
  uint16_t hue;
  uint16_t saturation;
  uint16_t brightness;
  uint16_t kelvin;
  
  uint32_t duration;
} lifx_payload_light_set_color;
#pragma pack(pop)


#define WAVEFORM_SAW = 0
#define WAVEFORM_SINE = 1
#define WAVEFORM_HALF_SINE = 2
#define WAVEFORM_TRIANGLE = 3
#define WAVEFORM_PULSE = 4

// see SetWaveform https://lan.developer.lifx.com/docs/changing-a-device#setwaveform---packet-103
#pragma pack(push, 1)
typedef struct {
  uint8_t reserved;
  //HSBK
  uint16_t hue;
  uint16_t saturation;
  uint16_t brightness;
  uint16_t kelvin;
  //
  uint32_t period;
  float cycles;
  int16_t skew_ratio;
  uint8_t waveform;
} lifx_payload_light_set_waveform;
#pragma pack(pop)



// Payload types
#define LIFX_DEVICE_SETPOWER 21
#define LIFX_LIGHT_SETCOLOR 102
#define LIFX_LIGHT_SETWAVEFORM 103
static const char *const TAG = "lifx";

void set_color(const unsigned int hue, const unsigned int saturation, const unsigned int kelvin) {
    if (!network::is_connected()) {
      return;
    }

    // Set color to purple

    lifx_header header;
    lifx_payload_light_set_color payload;
    
    // Initialise both structures
    memset(&header, 0, sizeof(header));
    memset(&payload, 0, sizeof(payload));
    
    // Setup the header
    header.size = sizeof(lifx_header) + sizeof(payload); // Size of header + payload
    header.tagged = 1;
    header.addressable = 1;
    header.protocol = 1024;
    header.source = 123;
    header.ack_required = 0;
    header.sequence = 100;
    header.type = LIFX_LIGHT_SETCOLOR;
    
   
    payload.hue = hue; // 0 - 65535
    payload.saturation = saturation;
    payload.brightness = 65535;
    
    payload.kelvin = kelvin;
    
    // Send a packet on startup
    Udp.beginPacket(notification_ip, 56700);
    Udp.write((const uint8_t *) &header, sizeof(lifx_header)); // Treat the structures like byte arrays
    Udp.write((const uint8_t *) &payload, sizeof(payload));    // Which makes the data on wire correct
    Udp.endPacket();
}

void set_waveform(const unsigned int hue, const unsigned int saturation, const unsigned int kelvin, const uint32_t period, const float cycles, const int16_t skew_ratio, const uint8_t waveform) {
    if (!network::is_connected()) {
      return;
    }

    // Set color to purple

    lifx_header header;
    lifx_payload_light_set_waveform payload;
    
    // Initialise both structures
    memset(&header, 0, sizeof(header));
    memset(&payload, 0, sizeof(payload));
    
    // Setup the header
    header.size = sizeof(lifx_header) + sizeof(payload); // Size of header + payload
    header.tagged = 1;
    header.addressable = 1;
    header.protocol = 1024;
    header.source = 123;
    header.ack_required = 0;
    header.sequence = 100;
    header.type = LIFX_LIGHT_SETWAVEFORM;
    
   
    payload.hue = hue; // 0 - 65535
    payload.saturation = saturation;
    payload.brightness = 65535;
    payload.kelvin = kelvin;
    payload.period = period;
    payload.cycles = cycles;
    payload.skew_ratio = skew_ratio;
    payload.waveform = waveform;
    
    // Send a packet on startup
    Udp.beginPacket(notification_ip, 56700);
    Udp.write((const uint8_t *) &header, sizeof(lifx_header)); // Treat the structures like byte arrays
    Udp.write((const uint8_t *) &payload, sizeof(payload));    // Which makes the data on wire correct
    Udp.endPacket();
}