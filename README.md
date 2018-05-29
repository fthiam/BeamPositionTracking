---------------------
Beam Position Trackin Project
---------------------
BeamPositionTracking is an application based on Tango Devices (http://www.tango-controls.org/) that will be used on beamlines to enslave the beam position on a specific target. 

The back-end is made of two Tango Devices :
* ActuatorSystem :  This device's aim is to organize translations (X and Y axis). 
* BeamPositionTracking : In order to move the beam centroid, BeamPositionTracking device will need to work with ActuatorSystem (pleaser refer to AS documentation). Asservissement loop is based on a PID algorithm (on per axis). 

Theses devices will be able to work with differents types of sub-devices for translations and several detectors such as Lima detector and XBPM using Yat4Tango::Plugin technology.


---------------------
Falilou THIAM
falilou.thiam@synchrotron-soleil.fr
---------------------
