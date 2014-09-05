opgateway
=========

**THIS PROJECT NOT DEVELOPED ANYMORE.**
Since the hardware is not available for me, I went to the common autopilots with MAVLink protocol and wrote mavros package.


ROS node for communicate with OpenPilot system.

Depends on OpenPilot uavobjdenerator from https://github.com/vooon/OpenPilot vooon/uavobjgen-ros branch.

Features
--------

  * Communication with AutoPilot using serial (e.g. hardware UARTs, USB-UART and etc)
  * UDP Relay for OpenPilot Ground Control Station
  * Plugin system for ROS-UAVObject communication (TODO)


Limitations
-----------

Only for linux.
Tested on Ubuntu ARM 13.04 armhf.


Links
-----

  * [OpenPilot][1] - mainline developer tree
  * [Globalcontrol][2] - initial idea how i could write ros node
  * [TauLabs][3] - the source of UavTalk Relay (and base for GlobalControl)


[1]: http://git.openpilot.org/browse/OpenPilot
[2]: https://github.com/fhp/TauLabs.git
[3]: https://github.com/TauLabs/TauLabs.git
