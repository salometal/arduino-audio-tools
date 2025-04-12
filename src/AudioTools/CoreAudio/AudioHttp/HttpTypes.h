#pragma once

// Escludi le funzionalità HTTP se AUDIO_TOOLS_EXCLUDE_HTML è definito
#ifndef AUDIO_TOOLS_EXCLUDE_HTML
/** @brief supported http methods */
enum MethodID {
  UNDEFINED,
  GET,
  HEAD,
  POST,
  PUT,
  DELETE,
  TRACE,
  OPTIONS,
  CONNECT,
  PATCH
};
#endif