/*
 * MktrDate.cpp
 *
 *  Created on: Sep 1, 2010
 *      Author: xludesi
 */

#include "MktrDate.h"
//#include <windows.h>

#include <time.h>
#include <string.h>
#include <stdio.h>


MktrDate::~MktrDate()
{
	// TODO Auto-generated destructor stub
}

MktrDate::MktrDate()
{
   // SYSTEMTIME st;
   // ::GetLocalTime(&st);
    time_t now = time(NULL);
    struct tm * loTime = ::localtime(&now);

    if(loTime != NULL)
    {
    	//iyear = st.wYear;
    	_iyear = loTime->tm_year + 1900;
		//imonth = st.wMonth;
		_imonth = loTime->tm_mon + 1;
		//iday = st.wDay;
		_iday = loTime->tm_mday;
		//ihour = st.wHour;
		_ihour = loTime->tm_hour;
		//iminute = st.wMinute;
		_iminute = loTime->tm_min;
    }
    else
    {
    	_iyear=0;
    	_imonth=0;
    	_iday=0;
    	_ihour=0;
    	_iminute=0;
    }
}


MktrDate::MktrDate(const time_t time)
{
    int tDayL, loDayL;
    time_t nTime = ::time(NULL);
    struct tm *loTime = ::localtime(&nTime);
    loDayL = loTime->tm_isdst;

    struct tm *pTime = localtime(&time);
    tDayL = pTime->tm_isdst;

    _iyear = pTime->tm_year+1900;
    _imonth = pTime->tm_mon+1;
    _iday = pTime->tm_mday;

    if(loDayL) // Local daylight saving time ?
    {
	    if(tDayL) // Event daylight saving time ?
	    {
            _ihour = pTime->tm_hour;
			_iminute = pTime->tm_min;
        }
   	    else // Add one hour
	    {
            _ihour = pTime->tm_hour+1;
			_iminute = pTime->tm_min;

	    }
    }
    else
    {
	    if (tDayL) //Remove one hour
	    {
            _ihour = pTime->tm_hour-1;
			_iminute = pTime->tm_min;
	    }
	    else
	    {
            _ihour = pTime->tm_hour;
			_iminute = pTime->tm_min;
	    }
    }
}

MktrDate::MktrDate(const unsigned int year, const unsigned int month, const unsigned int day, const unsigned int hour, const unsigned int minute)
{
    _iyear = year;
    _imonth = month;
    _iday = day;
    _ihour = hour;
    _iminute = minute;
}

MktrDate::MktrDate(const MktrDate& orig)
{
    this->_iyear = orig._iyear;
    this->_imonth = orig._imonth;
    this->_iday = orig._iday;
    this->_ihour = orig._ihour;
    this->_iminute = orig._iminute;
}

MktrDate& MktrDate::operator=(const MktrDate& orig)
{
    this->_iyear = orig._iyear;
    this->_imonth = orig._imonth;
    this->_iday = orig._iday;
    this->_ihour = orig._ihour;
    this->_iminute = orig._iminute;
    return *this;
}

MktrDate* MktrDate::create(const unsigned int year, const unsigned int month, const unsigned int day, const unsigned int hour, const unsigned int minute)
{
    unsigned int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int _iyear = year;
    if(_iyear < 100)
    {
        MktrDate date;
        _iyear += date._century();
    }

    if((_iyear % 400 == 0) || ((_iyear % 100 != 0) && (_iyear % 4 == 0)))
    {
        days[1] = 29;
    }

    if(month > 0 && month <13)
    {
        if(day > 0 && day <= days[month-1])
        {
            if((int)hour >= 0 && hour < 24)
            {
                if((int)minute >= 0 && minute < 60)
                {
                    return new MktrDate(_iyear, month, day, hour, minute);
                }
            }
        }
    }
    return NULL;
}

void MktrDate::moveForward(const unsigned long minutes)
{
    time_t seconds = minutes*60;

    time_t now;
	::time(&now);
	struct tm *timeStruct = ::localtime(&now);
	::memset(timeStruct, '\0', sizeof(struct tm));

    timeStruct->tm_year = this->_iyear - 1900;
	timeStruct->tm_mon  = this->_imonth - 1;
	timeStruct->tm_mday = this->_iday;
	timeStruct->tm_hour = this->_ihour;
	timeStruct->tm_min  = this->_iminute;
	timeStruct->tm_isdst = -1;

    time_t ct = ::mktime(timeStruct);
    time_t nt = ct+seconds;

    timeStruct = ::localtime(&nt);
    this->_iyear = timeStruct->tm_year + 1900;
    this->_imonth = timeStruct->tm_mon + 1;
    this->_iday = timeStruct->tm_mday;
    this->_ihour = timeStruct->tm_hour;
    this->_iminute = timeStruct->tm_min;
}

void MktrDate::moveBackword(const unsigned long minutes)
{
    time_t seconds = minutes*60;

    time_t now;
	::time(&now);
	struct tm *timeStruct = ::localtime(&now);
	::memset(timeStruct, '\0', sizeof(struct tm));

    timeStruct->tm_year = _iyear - 1900;
	timeStruct->tm_mon  = _imonth - 1;
	timeStruct->tm_mday = _iday;
	timeStruct->tm_hour = _ihour;
	timeStruct->tm_min  = _iminute;
	timeStruct->tm_isdst = -1;

    time_t ct = ::mktime(timeStruct);
    time_t nt = ct-seconds;

    timeStruct = ::localtime(&nt);
    _iyear = timeStruct->tm_year + 1900;
    _imonth = timeStruct->tm_mon + 1;
    _iday = timeStruct->tm_mday;
    _ihour = timeStruct->tm_hour;
    _iminute = timeStruct->tm_min;
}

int MktrDate::_century()
{
    return (_iyear/100)*100;
}

int MktrDate::year() const
{
    return _iyear;
}

int MktrDate::month() const
{
    return _imonth;
}

int MktrDate::day() const
{
    return _iday;
}

int MktrDate::hour() const
{
    return _ihour;
}

int MktrDate::minute() const
{
    return _iminute;
}

bool MktrDate::operator==(const MktrDate& right)
{
    return (_compareDates(*this, right) == 0);
}
bool MktrDate::operator!=(const MktrDate& right)
{
    return (_compareDates(*this, right) != 0);
}
bool MktrDate::operator<(const MktrDate& right)
{
    return (_compareDates(*this, right) > 0);
}
bool MktrDate::operator<=(const MktrDate& right)
{
    return (_compareDates(*this, right) >= 0);
}
bool MktrDate::operator>(const MktrDate& right)
{
    return (_compareDates(*this, right) < 0);
}
bool MktrDate::operator>=(const MktrDate& right)
{
    return (_compareDates(*this, right) <= 0);
}


int MktrDate::_compareDates(const MktrDate& left, const MktrDate right)
{
    if(left._iyear > right._iyear)
    {
        return -1;
    }
    else if (left._iyear < right._iyear)
    {
        return 1;
    }
    else
    {
        if(left._imonth > right._imonth)
        {
            return -1;
        }
        else if (left._imonth < right._imonth)
        {
            return 1;
        }
        else
        {
            if(left._iday > right._iday)
            {
                return -1;
            }
            else if (left._iday < right._iday)
            {
                return 1;
            }
            else
            {
                if(left._ihour > right._ihour)
                {
                    return -1;
                }
                else if (left._ihour < right._ihour)
                {
                    return 1;
                }
                else
                {
                    if(left._iminute > right._iminute)
                    {
                        return -1;
                    }
                    else if (left._iminute < right._iminute)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }
        }
    }
}


std::string MktrDate::to_string()
{
	char buff[256];

	sprintf(buff, "(Year = %d, Month = %d, Day = %d, Hour = %d, Minute = %d)", year(), month(), day(), hour(), minute());

	return buff;
}
