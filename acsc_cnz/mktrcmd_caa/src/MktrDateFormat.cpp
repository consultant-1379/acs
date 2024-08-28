/*
 * MktrDateFormat.cpp
 *
 *  Created on: Sep 1, 2010
 *      Author: xludesi
 */

#include "MktrDateFormat.h"
#include <stdlib.h>
#include <sstream>


MktrDateFormat::~MktrDateFormat()
{
	// TODO Auto-generated destructor stub
}

MktrDateFormat::MktrDateFormat(const MktrDateFormat::Format fmt)
{
    this->_fmt = fmt;
}


MktrDate* MktrDateFormat::parse(const std::string& dateStr)
{
    if(_fmt == YYMMDD_HHmm)
    {
        if(dateStr.size() != 11 || dateStr[6] != '-')
        {
            return NULL;
        }

        for(unsigned int i=0; i<dateStr.size(); i++)
        {
            if (i != 6)
            {
                if((dateStr[i] < '0' || dateStr[i] > '9'))
                {
                    return NULL;
                }
            }
        }
        char yy[3];
        yy[0] = dateStr[0];
        yy[1] = dateStr[1];
        yy[2] = '\0';

        char MM[3];
        MM[0] = dateStr[2];
        MM[1] = dateStr[3];
        MM[2] = '\0';

        char dd[3];
        dd[0] = dateStr[4];
        dd[1] = dateStr[5];
        dd[2] = '\0';

        char hh[3];
        hh[0] = dateStr[7];
        hh[1] = dateStr[8];
        hh[2] = '\0';

        char mm[3];
        mm[0] = dateStr[9];
        mm[1] = dateStr[10];
        mm[2] = '\0';

        return MktrDate::create(atoi(yy), atoi(MM), atoi(dd), atoi(hh), atoi(mm));
    }
    else if(_fmt == YYYYMMDDHHmm)
    {
        if(dateStr.size() != 12)
        {
            return NULL;
        }

        for(unsigned int i=0; i<dateStr.size(); i++)
        {
            if((dateStr[i] < '0' || dateStr[i] > '9'))
            {
                return NULL;
            }
        }
        char yy[5];
        yy[0] = dateStr[0];
        yy[1] = dateStr[1];
        yy[2] = dateStr[2];
        yy[3] = dateStr[3];
        yy[4] = '\0';

        char MM[3];
        MM[0] = dateStr[4];
        MM[1] = dateStr[5];
        MM[2] = '\0';

        char dd[3];
        dd[0] = dateStr[6];
        dd[1] = dateStr[7];
        dd[2] = '\0';

        char hh[3];
        hh[0] = dateStr[8];
        hh[1] = dateStr[9];
        hh[2] = '\0';

        char mm[3];
        mm[0] = dateStr[10];
        mm[1] = dateStr[11];
        mm[2] = '\0';

        return MktrDate::create(atoi(yy), atoi(MM), atoi(dd), atoi(hh), atoi(mm));
    }
    else if(_fmt == YYMMDD)
    {
        if(dateStr.size() != 6)
        {
            return NULL;
        }

        for(unsigned int i=0; i<dateStr.size(); i++)
        {
            if((dateStr[i] < '0' || dateStr[i] > '9'))
            {
                return NULL;
            }
        }
        char yy[3];
        yy[0] = dateStr[0];
        yy[1] = dateStr[1];
        yy[2] = '\0';

        char MM[3];
        MM[0] = dateStr[4];
        MM[1] = dateStr[5];
        MM[2] = '\0';

        char dd[3];
        dd[0] = dateStr[6];
        dd[1] = dateStr[7];
        dd[2] = '\0';

        return MktrDate::create(atoi(yy), atoi(MM), atoi(dd), 0, 0);
    }
    else if(_fmt == HHmm)
    {
        if(dateStr.size() != 4)
        {
            return NULL;
        }

        for(unsigned int i=0; i<dateStr.size(); i++)
        {
            if((dateStr[i] < '0' || dateStr[i] > '9'))
            {
                return NULL;
            }
        }
        char hh[3];
        hh[0] = dateStr[8];
        hh[1] = dateStr[9];
        hh[2] = '\0';

        char mm[3];
        mm[0] = dateStr[10];
        mm[1] = dateStr[11];
        mm[2] = '\0';

        MktrDate today;

        return MktrDate::create(today.year(), today.month(), today.day(), atoi(hh), atoi(mm));
    }
    else
    {
        return NULL;
    }
}

std::string MktrDateFormat::format(const MktrDate& date)
{
    std::stringstream buf;
    if(_fmt == YYMMDD_HHmm)
    {
        buf.fill('0');
        buf.width(2);
        buf<<(date.year()%100);
        buf.fill('0');
        buf.width(2);
        buf<<date.month();
        buf.fill('0');
        buf.width(2);
        buf<<date.day();
        buf<<"-";
        buf.fill('0');
        buf.width(2);
        buf<<date.hour();
        buf.fill('0');
        buf.width(2);
        buf<<date.minute();
    }
    else if(_fmt == YYYYMMDDHHmm)
    {
        buf.fill('0');
        buf.width(4);
        buf<<date.year();
        buf.fill('0');
        buf.width(2);
        buf<<date.month();
        buf.fill('0');
        buf.width(2);
        buf<<date.day();
        buf.fill('0');
        buf.width(2);
        buf<<date.hour();
        buf.fill('0');
        buf.width(2);
        buf<<date.minute();
    }
    else if(_fmt == YYMMDD)
    {
        buf.fill('0');
        buf.width(2);
        buf<<(date.year()%100);
        buf.fill('0');
        buf.width(2);
        buf<<date.month();
        buf.fill('0');
        buf.width(2);
        buf<<date.day();
    }
    return buf.str();
}
