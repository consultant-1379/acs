//#include "stdafx.h"
#include "csadmexception.h"



CsadmException::CsadmException(errorCode code, string message)
{
	m_code = code;
	m_message = message;
}

const string CsadmException::getMessage()
{
	return m_message;
}

CsadmException::errorCode CsadmException::getCode()
{
	return m_code;
}
