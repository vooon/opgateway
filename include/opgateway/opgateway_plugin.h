#ifndef OPGATEWAY_PLUGIN_H
#define OPGATEWAY_PLUGIN_H

// Don't expose objmanager from lib uavobjects
namespace openpilot
{
class UAVObjectManager;
}

namespace opgateway
{

class OPGatewayPluginBase {
public:
	virtual void initialize(UAVObjectManager *objMngr);
	virtual ~OPGatewayPluginBase();

protected:
	OPGatewayPluginBase() {};
};

} // namespace opgateway

#endif // OPGATEWAY_PLUGIN_H
