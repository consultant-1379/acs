// Copyright Ericsson AB 2007. All rights reserved.

#include "ACS_CS_API.h"
#include "ACS_CS_Table.h"
#include "ACS_CS_API_TableLoader.h"
#include "ACS_CS_Util.h"
#include "ACS_CS_API_Internal.h"
#include "ACS_CS_API_CP_Implementation.h"

#include "ACS_CS_ImUtils.h"
#include "ACS_CS_ImModel.h"
#include "ACS_CS_ImTypes.h"
#include "ACS_CS_ImIMMReader.h"
#include "ACS_CS_ImModelSaver.h"
#include "ACS_CS_ImFunction.h"

// ACS_CS_API_FunctionDistribution_Implementation
#include "ACS_CS_API_FunctionDistribution_Implementation.h"

#include "ACS_CS_API_Tracer.h"

#include <string>

ACS_CS_API_TRACER_DEFINE(ACS_CS_API_FunctionDistribution_TRACE);

// Class ACS_CS_API_FunctionDistribution_Implementation 

ACS_CS_API_FunctionDistribution_Implementation::ACS_CS_API_FunctionDistribution_Implementation()
{
	m_FuncDistModel = new (std::nothrow) ACS_CS_ImModel();
	if(0 != m_FuncDistModel)
	{
		ACS_CS_ImIMMReader* immReader = new (std::nothrow) ACS_CS_ImIMMReader();

		if( (0 != immReader) )
		{
			immReader->loadModel(m_FuncDistModel, ACS_CS_ImmMapper::RDN_FUNCTION_DIST, ACS_APGCC_SUBTREE);
			delete immReader;
		}
	}
}

ACS_CS_API_FunctionDistribution_Implementation::~ACS_CS_API_FunctionDistribution_Implementation()
{
	if(0 != m_FuncDistModel)
		delete m_FuncDistModel;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_FunctionDistribution_Implementation::getFunctionNames (ACS_CS_API_NameList_R1 &functionList)
{
	ACS_CS_API_TRACER_MESSAGE("entered");

	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Success;

	// Check if the Function Distribution model has been load
	if(0 != m_FuncDistModel)
	{
		// Get all defined Function
	    std::set<const ACS_CS_ImBase*> functionObjects;
	    m_FuncDistModel->getObjects(functionObjects, APFUNCTION_T);

	    functionList.setSize(functionObjects.size());

	    uint16_t idx = 0U;
	    std::set<const ACS_CS_ImBase*>::const_iterator funcIter;

	    // For each Function get its Function Name (domain:function)
	    for(funcIter = functionObjects.begin(); funcIter != functionObjects.end(); ++funcIter)
	    {
	    	const ACS_CS_ImFunction* functionObj = dynamic_cast<const ACS_CS_ImFunction*>(*funcIter);

	    	// Check dynamic cast result
		    if(0 != functionObj)
		    {
		    	std::string functionName;
		    	// Get function name
			    if(functionObj->getFunctionName(functionName))
			    {
			    	// Insert the function name into returned list
			    	ACS_CS_API_Name tmpFunctionName(functionName.c_str());
				    functionList.setValue(tmpFunctionName, idx);
				    ++idx;
			    }
			    else
			    {
			    	ACS_CS_API_TRACER_MESSAGE("failed to get function name of object<%s>", functionObj->getFunctionDN() );
			    	resultValue = ACS_CS_API_NS::Result_Failure;
			    	// reset the size to zero as we seems to have some problem fetching the values
			    	functionList.setSize(0U);
			    	break;
			    }
		    }
		    else
		    {
		    	resultValue = ACS_CS_API_NS::Result_Failure;
			    // reset the size to zero as we seems to have some problem fetching the values
			    functionList.setSize(0U);
			    ACS_CS_API_TRACER_MESSAGE("failed cast to function object<%s>", (*funcIter)->rdn.c_str() );
			    break;
		    }
	   }
	}
	else
	{
		ACS_CS_API_TRACER_MESSAGE("model object is null");
		resultValue = ACS_CS_API_NS::Result_Failure;
	}

	ACS_CS_API_TRACER_MESSAGE("exists, result:<%d>", resultValue );
	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_FunctionDistribution_Implementation::getFunctionProviders (const ACS_CS_API_Name_R1 &name, ACS_CS_API_IdList_R1 &apList)
{
	ACS_CS_API_TRACER_MESSAGE("Entered");

	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

	if( 0 != m_FuncDistModel)
	{
		std::string functionNameToFind;

		// Get domain:service string
		if(getFunctionNameAsString(name, functionNameToFind))
		{
			ACS_CS_API_TRACER_MESSAGE("search function:<%s>", functionNameToFind.c_str() );

			// Get all defined Function
			std::set<const ACS_CS_ImBase*> functionObjects;
			m_FuncDistModel->getObjects(functionObjects, APFUNCTION_T);

			resultValue = ACS_CS_API_NS::Result_NoEntry;

			std::set<const ACS_CS_ImBase*>::const_iterator funcIter;
			// Loop through Function objects
			for(funcIter = functionObjects.begin(); funcIter != functionObjects.end(); ++funcIter)
			{
				const ACS_CS_ImFunction* functionObj = dynamic_cast<const ACS_CS_ImFunction*>(*funcIter);

				// Check dynamic cast result
				if(0 != functionObj)
				{
					std::string definedFunctionName;
					// Get current iterator function name
					if(functionObj->getFunctionName(definedFunctionName))
					{
						ACS_CS_API_TRACER_MESSAGE("defined function:<%s>", definedFunctionName.c_str() );
						// Check if equal
						if( definedFunctionName.compare(functionNameToFind) == 0)
						{
							// function found get the AP id where it is defined
							uint16_t apId;
							if(functionObj->getAPNodeId(apId))
							{
								apList.setSize(1U);
								apList.setValue(apId, 0U);
								resultValue = ACS_CS_API_NS::Result_Success;
								ACS_CS_API_TRACER_MESSAGE("function founded");
							}
							else
							{
								ACS_CS_API_TRACER_MESSAGE("failed to get AP id");
							}

							break;
						}
					}
					else
					{
						ACS_CS_API_TRACER_MESSAGE("failed to get function name of object<%s>", functionObj->getFunctionDN() );
					}
				}
				else
				{
					ACS_CS_API_TRACER_MESSAGE("failed cast to function object<%s>", (*funcIter)->rdn.c_str() );
				}
			}// end for loop
		}
		else
		{
			ACS_CS_API_TRACER_MESSAGE("failed to get function name");
		}
	}
	else
	{
		ACS_CS_API_TRACER_MESSAGE("model object is null");
	}

	ACS_CS_API_TRACER_MESSAGE("result:<%d>", resultValue );
	return resultValue;
}

ACS_CS_API_NS::CS_API_Result ACS_CS_API_FunctionDistribution_Implementation::getFunctionUsers(APID apid, const ACS_CS_API_Name_R1& name, ACS_CS_API_IdList_R1& cpList)
{
	ACS_CS_API_TRACER_MESSAGE("Entered");

	ACS_CS_API_NS::CS_API_Result resultValue = ACS_CS_API_NS::Result_Failure;

	if( 0 != m_FuncDistModel)
	{
		std::string functionNameToFind;

		// Get domain:service string
		if(getFunctionNameAsString(name, functionNameToFind))
		{
			ACS_CS_API_TRACER_MESSAGE("search function:<%s>", functionNameToFind.c_str() );

			// Get all defined Function
			std::set<const ACS_CS_ImBase*> functionObjects;
			m_FuncDistModel->getObjects(functionObjects, APFUNCTION_T);

			resultValue = ACS_CS_API_NS::Result_NoEntry;

			std::set<const ACS_CS_ImBase*>::const_iterator funcIter;
			// Loop through Function objects
			for(funcIter = functionObjects.begin(); funcIter != functionObjects.end(); ++funcIter)
			{
				const ACS_CS_ImFunction* functionObj = dynamic_cast<const ACS_CS_ImFunction*>(*funcIter);

				// Check dynamic cast result
				if(0 != functionObj)
				{
					std::string definedFunctionName;
					// Get current iterator function name
					if(functionObj->getFunctionName(definedFunctionName))
					{
						ACS_CS_API_TRACER_MESSAGE("defined function:<%s>", definedFunctionName.c_str() );
						// Check if equal
						if( definedFunctionName.compare(functionNameToFind) == 0)
						{
							ACS_CS_API_TRACER_MESSAGE("function founded");

							// function found get the APid
							if(functionObj->getAPNodeId() == apid)
							{
								// Get a list of all CP identities
								ACS_CS_API_CP* cpTable = ACS_CS_API::createCPInstance();
								resultValue = cpTable->getCPList(cpList);
								ACS_CS_API::deleteCPInstance(cpTable);

								ACS_CS_API_TRACER_MESSAGE("Cp list ready");
							}
							break;
						}
					}
					else
					{
						ACS_CS_API_TRACER_MESSAGE("failed to get function name of object<%s>", functionObj->getFunctionDN() );
					}
				}
				else
				{
					ACS_CS_API_TRACER_MESSAGE("failed cast to function object<%s>", (*funcIter)->rdn.c_str() );
				}
			}// end for loop
		}
		else
		{
			ACS_CS_API_TRACER_MESSAGE("failed to get function name");
		}
	}
	else
	{
		ACS_CS_API_TRACER_MESSAGE("model object is null");
	}

	ACS_CS_API_TRACER_MESSAGE("exists, result:<%d>", resultValue );

	return resultValue;
}

bool ACS_CS_API_FunctionDistribution_Implementation::getFunctionNameAsString(const ACS_CS_API_Name_R1& name, std::string& functionName)
{
	ACS_CS_API_TRACER_MESSAGE("Entered");
	bool result = false;
	size_t nameLength = name.length();
	char nameChar[nameLength + 1];
	memset(nameChar, 0, nameLength + 1);

	// get Function name as "domain:function"
	if( ACS_CS_API_NS::Result_Success == name.getName(nameChar, nameLength) )
	{
		functionName.assign(nameChar);
		ACS_APGCC::toUpper(functionName);
		result = true;
	}

	ACS_CS_API_TRACER_MESSAGE("exists, functionName:<%s>, result=<%s>", functionName.c_str(), ( result ? "TRUE" : "FALSE" ));
	return result;
}
