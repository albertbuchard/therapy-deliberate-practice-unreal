#include "TherapyDP.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(LogTherapyDP);

class FTherapyDPModule : public IModuleInterface
{
public:
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FTherapyDPModule, TherapyDP)
