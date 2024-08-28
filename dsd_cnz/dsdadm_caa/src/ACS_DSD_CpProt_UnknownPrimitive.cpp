#include "ACS_DSD_CpProt_UnknownPrimitive.h"

CpProt_unknown_reply::CpProt_unknown_reply()
{
	_protocol_id = PROTOCOL_CPAP;
	_protocol_version = CPAP_PROTOCOL_VERSION_0;
	_primitive_id = CPAP_UNKNOWN_REPLY;
	_Unknown = 0;
}

std::string CpProt_unknown_reply::to_text() const
{
	char mess[ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN];
	snprintf(mess,ACS_DSD_PRIMITIVE_DATA_MAX_TEXT_DESCR_LEN,
				"PRIMITIVE_ID = %u,"
				"VERSION = %u,"
				"UNKNOWEN = %u",
				_primitive_id,
				_protocol_version,
				_Unknown);

	return mess;
}

int CpProt_unknown_reply::pack_into_primitive_data_handler(ACS_DSD_PrimitiveDataHandler<>  & pdh) const
{
	return  pdh.make_primitive(CPAP_UNKNOWN_REPLY, CPAP_PROTOCOL_VERSION_0, _Unknown);
}

int  CpProt_unknown_reply::build_from_primitive_data_handler(const ACS_DSD_PrimitiveDataHandler<> & pdh)
{
	//ACS_DSD_PrimitiveDataHandler<> & pdh_no_const = const_cast< ACS_DSD_PrimitiveDataHandler<> & >(pdh);
	//return pdh_no_const.unpack_primitive(_primitive_id, _protocol_version, &_Unknown);
	return pdh.unpack_primitive(_primitive_id, _protocol_version, &_Unknown);
}

int CpProt_unknown_reply::process(ACS_DSD_ServicePrimitive *& /*response_primitive*/, ACS_DSD_ServiceHandler */*service_handler*/) const
{
	return ACS_DSD_PRIM_PROCESS_OK_NO_RESPONSE;
}

