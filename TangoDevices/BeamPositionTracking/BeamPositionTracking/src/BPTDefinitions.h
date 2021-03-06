// ============================================================================
//
// = CONTEXT
//    TANGO Project -
//
// = FILENAME
//    BeamPositionTracking.h
//
// = AUTHOR
//    F. Thiam
//
// ============================================================================
#ifndef DEFINITIONS
#define DEFINITIONS

namespace BPTTaskManager{

	struct StateStatus{
		Tango::DevState state;
		std::string status;
	};

	struct WarningZone{
		double xCenter;
		double yCenter;
		double radius;
	};

	struct BeamDiagnostic{
		double xBeamPosition;
		double yBeamPosition;
		double xBeamPostionPixels;
		double yBeamPostionPixels;
		bool isBeamDetected;
		bool isBeamInZone;
		std::vector<unsigned char> thresholdedImg;
		int imgHeight;
		int imgWidth;
	};

	struct PIDCoefficient{
		bool isPIDEnabled;
		double P;
		double I;
		double D;
	};

	struct AxisInfo{
		std::string axisID;
		bool isAxisLinear;
		PIDCoefficient PIDCoefficients;
		std::string correctionTablePath;
		double stepNbToCalibrate;
	};

	struct Axis{
		AxisInfo axisInfo;
		bool isAxisCalibrated;
		double currentPosition;
		Tango::DevState currentState;
		ushort axisThreshold;
	};

	struct ManagerDataPacket{
		double computedTime;
		Axis xAxisData;
		Axis yAxisData;
		BeamDiagnostic beamDiagnostic;
	};

	struct Target{
		short xTargetPoint;
		short yTargetPoint;
	};

	struct FixedModeDefinition{
		bool isFixedModeEnabled;
		WarningZone warningZone;
		Target target;
	};
}
#endif