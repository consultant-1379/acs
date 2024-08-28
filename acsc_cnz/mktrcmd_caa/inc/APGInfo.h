#ifndef APGINFO_H_
#define APGINFO_H_

#include "MktrCommonDefs.h"
#include <string>

// Useful functions to get textual descriptions for some constants
const char * apgShelfArchitectureType_descr(MktrCommonDefs::ApgShelfArchitectureType apgShelfArchType);
const char * apzType_descr(MktrCommonDefs::ApzType apzType);

class APGInfo
{
public:
	APGInfo();
	virtual ~APGInfo();

	// gets APG information and stores it in member data. Returns : 0 on success, <> 0 on failure
	int load();

	MktrCommonDefs::ApgType apgType() { return _apgType; }
	MktrCommonDefs::ApType apType() { return _apType; }
	MktrCommonDefs::ApgShelfArchitectureType apgShelfArchitectureType() { return _apgShelfArchitectureType; }
	MktrCommonDefs::ApzType apzType() { return _apzType; }
	MktrCommonDefs::NodeType nodeStatus() { return _nodeStatus; }
	MktrCommonDefs::ApgHwGepType gepType() { return _apgHwGepType; }
	std::string getErrorDescr() { return _lastErrorDescr; }

	static int get_shelf_architecture_type(MktrCommonDefs::ApgShelfArchitectureType & apgShelfArchitectureType);

private:

	MktrCommonDefs::ApgType _apgType;
	MktrCommonDefs::ApType _apType;
	MktrCommonDefs::ApgShelfArchitectureType _apgShelfArchitectureType;
	MktrCommonDefs::ApzType _apzType;
	MktrCommonDefs::NodeType _nodeStatus;
	MktrCommonDefs::ApgHwGepType _apgHwGepType;
	std::string _lastErrorDescr;
	std::string _retrieveAPType;

	static MktrCommonDefs::ApgShelfArchitectureType _apg_shelf_arch_type;

	// helper methods
	int _getAPGType();
	int _getAPType();
	int _getApgShelfArchitectureType() { return get_shelf_architecture_type(_apgShelfArchitectureType);}
	int _getApzType();
	int _getNodeStatus();
	int _getApgHwGepType();
};

#endif /* APGINFO_H_ */
