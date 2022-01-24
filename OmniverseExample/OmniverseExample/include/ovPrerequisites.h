#pragma once

#include <OmniClient.h>
#include <OmniUsdLive.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/metrics.h>

#include <string>
#include <exception>
#include <cstdio>

PXR_NAMESPACE_USING_DIRECTIVE

namespace OV_SDK {

  using String = std::string;
  using int32 = uint32_t;

  // A C++ implementation of printf, or in other words a better printf
  template<typename ... Args>
  static String format(const String& format, Args ... args)
  {
    int32 size_s = _snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size_s <= 0) { ThrowRuntimeError("Error during formatting."); }
    auto size = static_cast<size_t>(size_s);
    auto buf = std::make_unique<char[]>(size);
    _snprintf(buf.get(), size, format.c_str(), args ...);
    return String(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
  }

  static
  void ThrowException(const String& message) {
    throw::std::exception(message.c_str());
  }

  static
  void ThrowRuntimeError(const String& message) {
    throw::std::runtime_error(message.c_str());
  }
}
