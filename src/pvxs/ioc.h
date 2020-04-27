/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * pvxs is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#ifndef PVXS_IOC_H
#define PVXS_IOC_H

#include <pvxs/server.h>

namespace pvxs {
namespace ioc {

/** Return Server instance registered with EPICS IOC
 *
 * IOC Server is initialized the first time it is referenced, either from this
 * function or from IOC shell command pvaStart. But only pvaStart will actually
 * start the Server.
 */
PVXS_API server::Server& getServer();

}; // namespace ioc
}; // namespace pvxs

#endif // PVXS_LOG_H
