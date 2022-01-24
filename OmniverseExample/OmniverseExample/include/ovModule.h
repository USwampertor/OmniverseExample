/*********************************************/
/*
 * @file 	ovModule
 * @author	Marco "Swampertor" Millan
 * @date	23/01/2022
 * @brief	
 *
 */
/******************************************** */

#pragma once

#include "ovPrerequisites.h"
#include <mutex>
#include <iostream>

namespace OV_SDK {

/**
 * This is a mutex used mainly for logging things from Omniverse. This will only
 * be active if the example is run with a -v or --verbose parameter, or is 
 * checked at the start. This is controlled by gOmniverseLoggingEnabled
 */
static std::mutex gLogMutex;

/**
 * This is to define if there is going to be logging from Omniverse or not
 */
static bool gOmniverseLoggingEnabled = false;

/**
 * @brief This is a callback that shows the connection status of Omniverse
 * @param void* userData: the data of the user connecting to Nucleus
 * @param const char* url: the URL being accessed, in this case, should start 
 *        with omniverse:// or omniverse://localhost/
 * @param OmniClientConnectionStatus status: the status of the connection
 * @return nothing
 */
static void
omniClientCallback(void* userData, 
                   const char* url, 
                   OmniClientConnectionStatus status) noexcept {
  // Let's just print this regardless
  {
    std::unique_lock<std::mutex> lk(gLogMutex);
    std::cout << "Connection Status: " << omniClientGetConnectionStatusString(status) << " [" << url << "]" << std::endl;
  }
  if (status == eOmniClientConnectionStatus_ConnectError)
  {
    // We shouldn't just exit here - we should clean up a bit, but we're going to do it anyway
    std::cout << "[ERROR] Failed connection, exiting." << std::endl;
    throw std::exception("[ERROR] Failed connection to Omniverse");
    // exit(-1);
  }
}

/**
 * Notifies that there was an error
 */
static void 
failNotify(const char* msg, const char* detail = nullptr, ...) {
  std::unique_lock<std::mutex> lk(gLogMutex);

  fprintf(stderr, "%s\n", msg);
  if (detail != nullptr)
  {
    fprintf(stderr, "%s\n", detail);
  }
}

// Omniverse Log callback
static void logCallback(const char* threadName, const char* component, OmniClientLogLevel level, const char* message) noexcept
{
  std::unique_lock<std::mutex> lk(gLogMutex);
  if (gOmniverseLoggingEnabled)
  {
    puts(message);
  }
}

class Module
{
public:
  // Default constructor
  Module() = default;

  // Default destructor
  ~Module() = default;

  /**
   * Initializes the omniverse module
   */
  bool
  init(bool doLiveEdit);

  /**
   * Shuts down the module
   */
  void
  shutdown();

  /**
   * Starts the connection to the Omniverse Nucleus client
   */
  bool
  startConnection();

  /**
   * Shuts down the connection to the Omniverse Nucleus Client
   */
  void
  endConnection();

  String
  createStage(const String& url, const String& stageName = "Sample.usd");

  void
  checkpointFile(const String& url, const String& comment);

  String
  getUserName(const String& url);

  bool 
  isValidOmniURL(const String& url);


  UsdStageRefPtr m_stage;
};

}
