#ifndef CSAPI_SET_h
#define CSAPI_SET_h 1


#include <string>

struct Result {

	Result() : result(ACS_CS_API_NS::Result_Failure) {}

	std::string value;
	int result;
};

Result getCPId(const std::vector<std::string> &arg);
Result getCPName(const std::vector<std::string> &arg);
Result getAPZType(const std::vector<std::string> &arg);
Result getCPList(const std::vector<std::string> &arg);
Result getAPZSystem(const std::vector<std::string> &arg);
Result getCPType(const std::vector<std::string> &arg);
Result getCPAliasName(const std::vector<std::string> &arg);
Result getState(const std::vector<std::string> &arg);
Result getApplicationId(const std::vector<std::string> &arg);
Result getApzSubstate(const std::vector<std::string> &arg);
Result getStateTransition(const std::vector<std::string> &arg);
Result getAptSubstate(const std::vector<std::string> &arg);
Result getCPQuorumData(const std::vector<std::string> &arg);
Result getBlockingInfo(const std::vector<std::string> &arg);
Result getCpCapacity(const std::vector<std::string> &arg);

Result getFunctionNames(const std::vector<std::string> &arg);
Result getFunctionProviders(const std::vector<std::string> &arg);
Result getFunctionUsers(const std::vector<std::string> &arg);

Result getMagazine(const std::vector<std::string> &arg);
Result getSlot(const std::vector<std::string> &arg);
Result getSysType(const std::vector<std::string> &arg);
Result getSysNo(const std::vector<std::string> &arg);
Result getFBN(const std::vector<std::string> &arg);
Result getSide(const std::vector<std::string> &arg);
Result getSeqNo(const std::vector<std::string> &arg);
Result getIPEthA(const std::vector<std::string> &arg);
Result getIPEthB(const std::vector<std::string> &arg);
Result getAliasEthA(const std::vector<std::string> &arg);
Result getAliasEthB(const std::vector<std::string> &arg);
Result getAliasNetmaskEthA(const std::vector<std::string> &arg);
Result getAliasNetmaskEthB(const std::vector<std::string> &arg);
Result getDhcpMethod(const std::vector<std::string> &arg);
Result getSysId(const std::vector<std::string> &arg);
Result getBoardIds(const std::vector<std::string> &arg);

Result isMultipleCPSystem(const std::vector<std::string> &arg);
Result isTestEnvironment(const std::vector<std::string> &arg);
Result getFrontAPG(const std::vector<std::string> &arg);
Result getNEID(const std::vector<std::string> &arg);
Result getSingleSidedCPCount(const std::vector<std::string> &arg);
Result getDoubleSidedCPCount(const std::vector<std::string> &arg);
Result getAPGCount(const std::vector<std::string> &arg);
Result getAlarmMaster(const std::vector<std::string> &arg);
Result getClockMaster(const std::vector<std::string> &arg);
Result getBSOMIPAddress(const std::vector<std::string> &arg);
Result getDefaultCPName(const std::vector<std::string> &arg);
Result getOmProfile(const std::vector<std::string> &arg);
Result getClusterOpMode(const std::vector<std::string> &arg);
Result getTrafficIsolated(const std::vector<std::string> &arg);
Result getTrafficLeader(const std::vector<std::string> &arg);
Result getAPTType(const std::vector<std::string> &arg);


#endif
