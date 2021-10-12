#include <ISmmPlugin.h>
#include "tier0/icommandline.h"

PLUGIN_GLOBALVARS();
SH_DECL_HOOK0(IServerGameDLL, GetTickInterval, const, 0, float);

// i know this is defined in const.h but i'm including it for code readability
#define DEFAULT_TICK_INT (0.015)
#define DEFAULT_TICKRATE (1.0 / DEFAULT_TICK_INT) // = 66.666etc

float Hook_GetTickInterval()
{
    float tickrate      = DEFAULT_TICKRATE;
    float tickinterval  = DEFAULT_TICK_INT;

    // check if the user defined a tickrate
    if (CommandLine()->CheckParm("-tickrate"))
    {
        // get the tickrate value
        tickrate = CommandLine()->ParmValue( "-tickrate", float(DEFAULT_TICKRATE) );

        // convert it to a tick int value (1/x)
        tickinterval = (1.0f / tickrate);

        // tell them that the values they set are getting set
        Warning("\n[TFTickrate] Tickrate set to %.1f, tick interval set to %f\n\n", tickrate, tickinterval);
    }
    else
    {
        // cry about it
        Warning("\n[TFTickrate] Tickrate not set, using default %.1f\n\n", tickrate);
    }

    // don't call the original func
    RETURN_META_VALUE(MRES_SUPERCEDE, tickinterval );
}

class TickRatePlugin : public ISmmPlugin
{
    public:
        bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);
        bool Unload(char *error, size_t maxlen);
    public:
        const char *GetAuthor();
        const char *GetName();
        const char *GetDescription();
        const char *GetURL();
        const char *GetLicense();
        const char *GetVersion();
        const char *GetDate();
        const char *GetLogTag();
};

TickRatePlugin g_TickRatePlugin;
PLUGIN_EXPOSE(TickRatePlugin, g_TickRatePlugin);

IServerGameDLL *server = NULL;

bool TickRatePlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
    PLUGIN_SAVEVARS();

    // don't need to check METAMOD_PLAPI_VERSION here
    GET_V_IFACE_ANY(GetServerFactory, server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
    // detour GetTickInt
    SH_ADD_HOOK_STATICFUNC(IServerGameDLL, GetTickInterval, server, Hook_GetTickInterval, false);

    // spew to user that they installed us properly
    Warning("\n[TFTickrate] Loaded.\n\n");

    return true;
}

bool TickRatePlugin::Unload(char *error, size_t maxlen)
{
    SH_REMOVE_HOOK_STATICFUNC(IServerGameDLL, GetTickInterval, server, Hook_GetTickInterval, false);
    return true;
}

const char *TickRatePlugin::GetLicense()        { return "MIT"; }
const char *TickRatePlugin::GetVersion()        { return "0.2.0"; }
const char *TickRatePlugin::GetDate()           { return __DATE__; }
const char *TickRatePlugin::GetLogTag()         { return "mms-unlocked-tickrate"; }
const char *TickRatePlugin::GetAuthor()         { return "ldesgoui, sappho.io"; }
const char *TickRatePlugin::GetDescription()    { return "Force a TF2 server's tickrate with the -tickrate command line parameter"; }
const char *TickRatePlugin::GetName()           { return "Unlocked Tickrate"; }
const char *TickRatePlugin::GetURL()            { return "https://github.com/ldesgoui/mms-unlocked-tickrate"; }
