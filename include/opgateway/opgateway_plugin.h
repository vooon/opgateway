#include <diagnostic_updater/diagnostic_updater.h>

#ifndef OPGATEWAY_PLUGIN_H
#define OPGATEWAY_PLUGIN_H

// Don't expose objmanager from lib uavobjects
namespace openpilot
{
class UAVObjectManager;
}

namespace opgateway
{

class OPGatewayPlugin {
public:
	virtual void init(openpilot::UAVObjectManager *objMngr);
	virtual void update(void);
	virtual diagnostic_updater::DiagnosticTask* get_diag_task(void) = NULL;

protected:
};

} // namespace opgateway

#endif // OPGATEWAY_PLUGIN_H
