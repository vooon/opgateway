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
	virtual ~OPGatewayPluginBase();
	virtual OPGatewayPluginBase() {};
	virtual void initialize(UAVObjectManager *objMngr);
	virtual void update(void);
	virtual void diag_update(void);

protected:
};

} // namespace opgateway

#endif // OPGATEWAY_PLUGIN_H
