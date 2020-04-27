/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * pvxs is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */

#include <atomic>

#include <initHooks.h>
#include <iocsh.h>
#include <epicsExit.h>
#include <epicsExport.h>
#include <epicsGuard.h>
#include <epicsMutex.h>

#include <pvxs/ioc.h>
#include <utilpvt.h>

namespace pvxs {
namespace ioc {

typedef epicsGuard<epicsMutex> Guard;

epicsMutex server_lock;
std::atomic<bool> server_initialized{false};
pvxs::server::Server ioc_server;

void stopServer(void*);

EPICS_ALWAYS_INLINE void initServer()
{
    if(server_initialized.exchange(true) == false) {
        epicsAtExit(&stopServer, 0);
        Guard G(server_lock);
        ioc_server = pvxs::server::Config::from_env().build();
    }
}

void startServer(void*)
{
    initServer();
    Guard G(server_lock);
    ioc_server.start();
}

void stopServer(void*)
{
    if(server_initialized) {
        Guard G(server_lock);
        ioc_server.stop();
    }
}

pvxs::server::Server& getServer()
{
    initServer();
    return ioc_server;
}

void initHookCb(initHookState state)
{
    if(server_initialized) {
        if(state==initHookAfterIocRunning) {
            Guard G(server_lock);
            ioc_server.start();
        } else if(state==initHookAtIocPause) {
            Guard G(server_lock);
            ioc_server.stop();
        }
    }
}

void registerPvxsIoc(void)
{
    static struct iocshFuncDef startServerFuncDef = {
        .name = "pvxsStart",
        .nargs = 0
    };
    iocshRegister(&startServerFuncDef, (void(*)(const iocshArgBuf*))&pvxs::ioc::startServer);

    static struct iocshFuncDef stopServerFuncDef = {
        .name = "pvxsStop",
        .nargs = 0
    };
    iocshRegister(&stopServerFuncDef, (void(*)(const iocshArgBuf*))&pvxs::ioc::stopServer);

    initHookRegister(&pvxs::ioc::initHookCb);
}

}; // namespace ioc
}; // namespace pvxs

extern "C" {
    using namespace pvxs::ioc;
    epicsExportRegistrar(registerPvxsIoc);
}
