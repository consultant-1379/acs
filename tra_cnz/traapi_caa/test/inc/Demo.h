/*
 * Demo.h
 *
 *  Created on: Mar 4, 2011
 *      Author: xgiopap
 */
#ifndef DEMO_H_
#define DEMO_H_

#include "ace/Task.h"

class Demo : public ACE_Task_Base{
public:
	Demo(std::string Subsystem);
	virtual ~Demo();
	virtual int svc ( void );
};

#endif /* DEMO_H_ */
