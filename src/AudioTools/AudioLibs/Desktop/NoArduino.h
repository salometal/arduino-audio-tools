#pragma once
/**
 * @file NoArduino.h
 * @author Phil Schatzmann
 * @brief If you want to use the framework w/o Arduino you need to provide the
 * implementation of a couple of classes and methods!
 * @version 0.1
 * @date 2022-09-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "AudioToolsConfig.h"
#ifdef IS_DESKTOP
#  error We should not get here!
#endif
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>  // std::max
#include <chrono>


#define IS_NOARDUINO

#ifndef PSTR
#define PSTR(fmt) fmt
#endif

#ifndef PI
#define PI 3.14159265359f
#endif

#ifndef INPUT
#define INPUT 0x0
#endif

#ifndef OUTPUT
#define OUTPUT 0x1
#endif

#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif

#ifndef HIGH
#define HIGH 0x1
#endif
#ifndef LOW
#define LOW 0x0
#endif

using namespace std;

enum PrintCharFmt { DEC=10, HEX=16 };

namespace audio_tools {

class Print {
 public:
#ifndef DOXYGEN
  virtual size_t write(uint8_t ch) {
    // not implememnted: to be overritten
    return 0;
  }

  virtual size_t write(const char *str) {
    return write((const uint8_t *)str, strlen(str));
  }

  virtual size_t write(const char *buffer, size_t size) {
    return write((const uint8_t *)buffer, size);
  }

  virtual int print(const char *msg) {
    int result = strlen(msg);
    return write(msg, result);
  }

  virtual int println(const char *msg = "") {
    int result = print(msg);
    write('\n');
    return result + 1;
  }

  virtual int println(float number) {
    char buffer[120];
    snprintf(buffer, 120, "%f", number);
    return println(buffer);
  }

  virtual int print(float number) {
    char buffer[120];
    snprintf(buffer, 120, "%f", number);
    return print(buffer);
  }

  virtual int print(int number) {
    char buffer[80];
    snprintf(buffer, 80, "%d", number);
    return print(buffer);
  }

  virtual int print(char c, PrintCharFmt spec) {
    char result[5];
    switch (spec) {
      case DEC:
        snprintf(result, 3, "%c", c);
        return print(result);
      case HEX:
        snprintf(result, 3, "%x", c);
        return print(result);
    }
    return -1;
  }

  int println(int value, PrintCharFmt fmt) {
    return print(value, fmt) + println();
  }

#endif

  virtual size_t write(const uint8_t *data, size_t len) {
    if (data == nullptr) return 0;
    for (size_t j = 0; j < len; j++) {
      write(data[j]);
    }
    return len;
  }

  virtual int availableForWrite() { return 1024; }

  virtual void flush() { /* Empty implementation for backward compatibility */ }

 protected:
  int _timeout = 10;
};

class Stream : public Print {

 public:
  virtual ~Stream() = default;
  virtual int available() { return 0; }
  virtual size_t readBytes(uint8_t *data, size_t len) { return 0; }
#ifndef DOXYGEN
  virtual int read() { return -1; }
  virtual int peek() { return -1; }
  virtual void setTimeout(size_t t) {}
  size_t readBytesUntil(char terminator, char *buffer, size_t length) {
	for (int j=0;j<length;j++){
		int val = read();
		if (val == -1) return j-1;
		if (val == terminator) return j;
		buffer[j] = val;
	}
	return length;
  };
  size_t readBytesUntil(char terminator, uint8_t *buffer, size_t length) {
    return readBytesUntil(terminator, (char *)buffer, length);
  }

#endif
  operator bool() { return true; }
};

class Client : public Stream {
 public:
  void stop() {};
  virtual int read(uint8_t *buffer, size_t len) { return 0; };
  virtual int read() { return 0; };
  bool connected() { return false; };
  bool connect(const char *ip, int port) { return false; }
  virtual operator bool() { return false; }
};

class HardwareSerial : public Stream {
 public:
  size_t write(uint8_t ch) override { return putchar(ch); }
  virtual operator bool() { return true; }
};

static HardwareSerial Serial;

/// Maps input to output values
inline long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

}  // namespace audio_tools

#if defined(ESP32)
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"  // needed for ESP Arduino < 2.0
#include "freertos/FreeRTOSConfig.h"

/// e.g. for AudioActions
inline int digitalRead(int pin) {
  printf("digitalRead:%d\n", pin);
  return gpio_get_level((gpio_num_t)pin);
}

inline void digitalWrite(int pin, int value) {
   gpio_set_level((gpio_num_t)pin, value);
}

inline void pinMode(int pin, int mode) {
  gpio_num_t gpio_pin = (gpio_num_t)pin;
  printf("pinMode(%d,%d)\n", pin, mode);

  gpio_reset_pin(gpio_pin);
  switch (mode) {
    case INPUT:
      gpio_set_direction(gpio_pin, GPIO_MODE_INPUT);
      break;
    case OUTPUT:
      gpio_set_direction(gpio_pin, GPIO_MODE_OUTPUT);
      break;
    case INPUT_PULLUP:
      gpio_set_direction(gpio_pin, GPIO_MODE_INPUT);
      gpio_set_pull_mode(gpio_pin, GPIO_PULLUP_ONLY);
      break;
    default:
      gpio_set_direction(gpio_pin, GPIO_MODE_INPUT_OUTPUT);
      break;
  }
}

inline void delay(uint32_t ms){ vTaskDelay(ms / portTICK_PERIOD_MS);}
inline uint32_t millis() {return (xTaskGetTickCount() * portTICK_PERIOD_MS);}
inline void delayMicroseconds(uint32_t ms) {esp_rom_delay_us(ms);}
inline uint64_t micros() { return xTaskGetTickCount() * portTICK_PERIOD_MS * 1000;}

// delay and millis has been defined
#define DESKTOP_MILLIS_DEFINED

#endif
