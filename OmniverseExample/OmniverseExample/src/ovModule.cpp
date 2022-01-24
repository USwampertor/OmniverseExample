#include "ovModule.h"

namespace OV_SDK {
  bool
  Module::init(bool doLiveEdit) {
    // Register a function to be called whenever the library wants to print something to a log
    omniClientSetLogCallback(logCallback);

    // The default log level is "Info", set it to "Debug" to see all messages
    omniClientSetLogLevel(eOmniClientLogLevel_Debug);

    // Initialize the library and pass it the version constant defined in OmniClient.h
    // This allows the library to verify it was built with a compatible version. It will
    // return false if there is a version mismatch.
    if (!omniClientInitialize(kOmniClientVersion))
    {
      return false;
    }

    omniClientRegisterConnectionStatusCallback(nullptr, omniClientCallback);

    // Enable live updates
    omniUsdLiveSetDefaultEnabled(doLiveEdit);

    return true;
  }

  void
  Module::shutdown() {
    // Calling this prior to shutdown ensures that all pending live updates complete.
    omniUsdLiveWaitForPendingUpdates();

    // The stage is a sophisticated object that needs to be destroyed properly.  
    // Since gStage is a smart pointer we can just reset it
    m_stage.Reset();

    //omniClientTick(1000);
    omniClientShutdown();
  }

  bool
  Module::startConnection() {
    return true;
  }

  void
  Module::endConnection() {

  }

  String
  Module::createStage(const String& url, const String& stageName) {
    std::string stageUrl = url + stageName;

    // Delete the old version of this file on Omniverse and wait for the operation to complete
    {
      std::unique_lock<std::mutex> lk(gLogMutex);
      std::cout << "Waiting for " << stageUrl << " to delete... " << std::endl;
    }
    omniClientWait(omniClientDelete(stageUrl.c_str(), nullptr, nullptr));
    {
      std::unique_lock<std::mutex> lk(gLogMutex);
      std::cout << "finished" << std::endl;
    }

    // Create this file in Omniverse cleanly
    m_stage = UsdStage::CreateNew(stageUrl);
    if (!m_stage)
    {
      failNotify("Failure to create model in Omniverse", stageUrl.c_str());
      return std::string();
    }

    {
      std::unique_lock<std::mutex> lk(gLogMutex);
      std::cout << "New stage created: " << stageUrl << std::endl;
    }

    // Always a good idea to declare your up-ness
    UsdGeomSetStageUpAxis(m_stage, UsdGeomTokens->y);

    return stageUrl;
  }

  void
  Module::checkpointFile(const String& url, const String& comment) {
    if (omniUsdLiveGetDefaultEnabled())
    {
      return;
    }

    bool bCheckpointsSupported = false;
    omniClientWait(omniClientGetServerInfo(url.c_str(), &bCheckpointsSupported,
      [](void* UserData, OmniClientResult Result, OmniClientServerInfo const* Info) noexcept
      {
        if (Result == eOmniClientResult_Ok && Info && UserData)
        {
          bool* bCheckpointsSupported = static_cast<bool*>(UserData);
          *bCheckpointsSupported = Info->checkpointsEnabled;
        }
      }));

    if (bCheckpointsSupported)
    {
      const bool bForceCheckpoint = true;
      omniClientWait(omniClientCreateCheckpoint(url.c_str(), comment.c_str(), bForceCheckpoint, nullptr,
        [](void* userData, OmniClientResult result, char const* checkpointQuery) noexcept
        {}));

      std::unique_lock<std::mutex> lk(gLogMutex);
      std::cout << "Adding checkpoint comment <" << comment << "> to stage <" << url << ">" << std::endl;
    }
  }

  String
  Module::getUserName(const String& url) {
    // Get the username for the connection
    String userName("_none_");
    omniClientWait(omniClientGetServerInfo(url.c_str(), &userName, [](void* userData, OmniClientResult result, struct OmniClientServerInfo const* info) noexcept
      {
        String* userName = static_cast<String*>(userData);
        if (userData && userName && info && info->username)
        {
          userName->assign(info->username);
        }
      }));
    {
      // std::unique_lock<std::mutex> lk(gLogMutex);
      // std::cout << "Connected username: " << userName << std::endl;
    }
    return userName;
  }

  bool 
  Module::isValidOmniURL(const String& maybeURL) {
    bool isValidURL = false;
    OmniClientUrl* url = omniClientBreakUrl(maybeURL.c_str());
    if (url->host && url->path &&
      (String(url->scheme) == String("omniverse") ||
        String(url->scheme) == String("omni")))
    {
      isValidURL = true;
    }
    omniClientFreeUrl(url);
    return isValidURL;
  }

}

