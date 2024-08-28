/*
 * MktrDateFormat.h
 *
 *  Created on: Sep 1, 2010
 *      Author: xludesi
 */

#ifndef MKTRDATEFORMAT_H_
#define MKTRDATEFORMAT_H_

#include "MktrDate.h"
#include <string>

class MktrDateFormat
{
public:
	enum Format
	{
		YYYYMMDDHHmm,
		YYMMDD_HHmm,
		YYMMDD,
		HHmm
	};

	MktrDateFormat(const MktrDateFormat::Format fmt);

	MktrDate* parse(const std::string& dateStr);

	std::string format(const MktrDate& date);

	virtual ~MktrDateFormat();

private:
	    MktrDateFormat::Format _fmt;
};

#endif /* MKTRDATEFORMAT_H_ */
