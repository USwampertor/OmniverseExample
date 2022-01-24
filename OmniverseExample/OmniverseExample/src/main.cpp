/*********************************************/
/*
 * @file 	main
 * @author	Marco "Swampertor" Millan
 * @date	23/01/2022
 * @brief	This is an example of Omniverse Client strongly based on the Hello World
 * example created by Nvidia, but more documented and showing things a bit more
 * nuclear. This mainly to explain how things work in the Omniverse system for
 * connectors
 *
 */
/******************************************** */

#include "ovModule.h"

// To keep things in order, a namespace was created
using namespace OV_SDK;

// Main should be self explanatory, but here we start up everything
int main(int argc, char* argv[])
{
  // Wrapped everything into a nice class that is created and initializes everything
  Module* m = new Module();
  bool doLiveEdit = true;
  std::string existingStage;
  std::string destinationPath = "omniverse://localhost/Users/";

  try
  {
    if (!m->init(doLiveEdit)) {
      throw std::exception("Error Initializing Omniverse");
    }
    const String stageUrl = m->createStage(destinationPath);
    std::cout << format("Connected user: %s", m->getUserName(stageUrl)) << std::endl;

  }
  catch (std::exception* e)
  {
    std::cout << format("An error occurred: %s", e->what());
  }
  





  m->shutdown();
  // Deleting and freeing the memory used by the Module
  delete(m);

  return 0;
}