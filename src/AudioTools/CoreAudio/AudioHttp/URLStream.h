#pragma once

// Escludi le funzionalità HTTP se AUDIO_TOOLS_EXCLUDE_HTML è definito
#ifndef AUDIO_TOOLS_EXCLUDE_HTML

#include "AudioToolsConfig.h"
#ifdef USE_URL_ARDUINO

#if defined(ESP32)
# include <Client.h>
# include <WiFi.h>
# include <WiFiClientSecure.h>
# include <esp_wifi.h>
#endif

#include "AudioTools/CoreAudio/AudioBasic/Str.h"
#include "AudioTools/CoreAudio/AudioHttp/AbstractURLStream.h"
#include "AudioTools/CoreAudio/AudioHttp/HttpRequest.h"
#include "AudioTools/CoreAudio/AudioHttp/ICYStreamT.h"
#include "AudioTools/CoreAudio/AudioHttp/URLStreamBufferedT.h"

namespace audio_tools {

// Classe URLStream
class URLStream : public AbstractURLStream {
 public:
  URLStream(int readBufferSize = DEFAULT_BUFFER_SIZE) {
    TRACED();
    setReadBufferSize(readBufferSize);
  }
  // Altri costruttori e metodi...

 protected:
  HttpRequest request;
  Str url_str;
  Url url;
  long size;
  long total_read;
  Vector<uint8_t> read_buffer{0};
  uint16_t read_buffer_size = DEFAULT_BUFFER_SIZE;
  uint16_t read_pos;
  uint16_t read_size;
  bool active = false;
  bool wait_for_data = true;
  const char* network = nullptr;
  const char* password = nullptr;
  Client* client = nullptr;

#ifdef USE_WIFI
  WiFiClient* clientInsecure = nullptr;
#endif
#ifdef USE_WIFI_CLIENT_SECURE
  WiFiClientSecure* clientSecure = nullptr;
#endif

  int clientTimeout = URL_CLIENT_TIMEOUT;
  unsigned long handshakeTimeout = URL_HANDSHAKE_TIMEOUT;
  bool is_power_save = false;

  // Altri metodi protetti...
};

using ICYStream = ICYStreamT<URLStream>;

#if defined(USE_CONCURRENCY)
using URLStreamBuffered = URLStreamBufferedT<URLStream>;
using ICYStreamBuffered = URLStreamBufferedT<ICYStream>;
#endif

}  // namespace audio_tools

#endif // USE_URL_ARDUINO
#endif // AUDIO_TOOLS_EXCLUDE_HTML