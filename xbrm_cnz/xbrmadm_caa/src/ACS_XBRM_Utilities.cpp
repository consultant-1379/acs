#include "ACS_XBRM_Utilities.h"
#include "ACS_XBRM_Logger.h"
#include "ACS_XBRM_Tracer.h"
#include "ACS_XBRM_ImmUtlis.h"
#include "aes_ohi_extfilehandler2.h"
#include "acs_apgcc_adminoperation.h"
#include <acs_apgcc_oihandler_V3.h>
#include <aes_ohi_filehandler.h>
#include "ACS_CS_API.h"

#include <iostream>
#include <set>
#include <cmath>

#define ACS_XBRM_INVALID_TIMEZONE 6 /*Invalid time zone*/
#define ACS_XBRM_STARTTIME_EXPIRED 7
#define stopTimeAttr "stopTime"
#define startTimeAttr "startTime"
#define ACS_XBRM_INVALID_TIME 23

ACS_XBRM_TRACE_DEFINE(ACS_XBRM_Utilities);

namespace ACS_XBRM_UTILITY
{
    char *reportProgressRdn[2] = {
        "id=1,SystemBrMsystemBrMId=1",
        "id=1,systemBrmBackupSchedulerId=1,SystemBrMsystemBrMId=1"};
    /*Align with */
    char *cpBackupName[6] = {
        "#",
        "#", // NOT applicable
        "CP1",
        "CP2",
        "", // CPNAME for cluster
        ""  // CPNAME for single Cp
    };
}

ACS_XBRM_Utilities::ACS_XBRM_Utilities()
{
    ACS_CC_ReturnType result = immHandler.Init();
    if (ACS_CC_FAILURE == result)
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "immHandler initialization failed");
}

ACS_XBRM_Utilities::~ACS_XBRM_Utilities()
{
    ACS_CC_ReturnType result = immHandler.Finalize();
    if (ACS_CC_FAILURE == result)
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "immHandler finalization failed");
}

int ACS_XBRM_Utilities::isValidTransferQueue(const string &transferQueue, string &transferQueueDn)
{
    ACS_XBRM_TRACE_FUNCTION;
    unsigned int errorCode;

    // define
    AES_OHI_ExtFileHandler2 extFileH(ACS_XBRM_UTILITY::SUBSYS.c_str(), ACS_XBRM_UTILITY::APPNAME.c_str());

    // attach
    errorCode = extFileH.attach();
    if (errorCode != AES_OHI_NOERRORCODE)
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s, AFS error = %s", getExitCodeText(ACS_XBRM_UTILITY::TQ_ATTACH_FAIL).c_str(), extFileH.getErrCodeText(errorCode));
        return ACS_XBRM_UTILITY::TQ_ATTACH_FAIL;
    }
    else
    {
        // verify if the transfer queue is defined
        errorCode = extFileH.fileTransferQueueDefined(transferQueue.c_str(), transferQueueDn);
        if (errorCode != AES_OHI_NOERRORCODE)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "extFileH.fileTransferQueueDefined() failed, error = %s", extFileH.getErrCodeText(errorCode));
            return ACS_XBRM_UTILITY::TQ_INTERNAL_ERROR;
        }
        // detach
        errorCode = extFileH.detach();
        if (errorCode != AES_OHI_NOERRORCODE)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s, AFS error = %s", getExitCodeText(ACS_XBRM_UTILITY::TQ_DETACH_FAILED).c_str(), extFileH.getErrCodeText(errorCode));
            return ACS_XBRM_UTILITY::TQ_DETACH_FAILED;
        }

        // verify if transferQueue is only initiating or not
        ACS_CC_ImmParameter initiatingDSet;
        initiatingDSet.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_INITIATING_DSET_NAME.c_str());
        immHandler.getAttribute(transferQueueDn.c_str(), &initiatingDSet);

        ACS_CC_ImmParameter respondingDSet;
        respondingDSet.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_RESPONDING_DSET_NAME.c_str());
        immHandler.getAttribute(transferQueueDn.c_str(), &respondingDSet);

        if (respondingDSet.attrValuesNum > 0)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Invalid Transfer Queue, errorText = %s", getExitCodeText(ACS_XBRM_UTILITY::TQ_INVALID_RESPONDING_DESTINATION_SET).c_str());
            return ACS_XBRM_UTILITY::TQ_INVALID_RESPONDING_DESTINATION_SET;
        }
        if (initiatingDSet.attrValuesNum <= 0)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Invalid Transfer Queue, errorText = %s", getExitCodeText(ACS_XBRM_UTILITY::TQ_INVALID_INITIATING_DESTINATION_SET).c_str());
            return ACS_XBRM_UTILITY::TQ_INVALID_INITIATING_DESTINATION_SET;
        }

        initiatingDSet.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_INIT_DSET_DN_NAME.c_str());
        immHandler.getAttribute(transferQueueDn.c_str(), &initiatingDSet);

        for (int i = 0; i < (int)initiatingDSet.attrValuesNum; i++)
        {
            // get children
            // check sftp
            std::vector<std::string> idsChildren;
            std::string curIDSDn = reinterpret_cast<char *>(initiatingDSet.attrValues[i]);
            immHandler.getChildren(curIDSDn.c_str(), ACS_APGCC_SUBLEVEL, &idsChildren);
            bool primaryIDSExist = false;
            for (int j = 0; j < (int)idsChildren.size(); j++)
            {
                size_t primaryIndex = idsChildren[j].find("primaryInitiatingDestinationId");
                size_t secondaryIndex = idsChildren[j].find("secondaryInitiatingDestinationId");
                if (primaryIndex == 0)
                {
                    ACS_CC_ImmParameter primaryInitDest;
                    primaryInitDest.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TRANSFER_PROTOCOL_NAME.c_str());
                    immHandler.getAttribute(idsChildren[j].c_str(), &primaryInitDest);
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "primaryInitDest isSFTP = %d", *reinterpret_cast<int *>(primaryInitDest.attrValues[0]) == 0 ? 1 : 0);
                    if (*reinterpret_cast<int *>(primaryInitDest.attrValues[0]) != 0)
                    { /*0 - SFTPV2*/
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Invalid Transfer Protocol, errorText = %s", getExitCodeText(ACS_XBRM_UTILITY::TQ_INVALID_TRANSFER_PROTOCOL).c_str());
                        return ACS_XBRM_UTILITY::TQ_INVALID_TRANSFER_PROTOCOL;
                    }
                    primaryIDSExist = true;
                }
                if (secondaryIndex == 0)
                {
                    ACS_CC_ImmParameter secondaryInitDest;
                    secondaryInitDest.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_TRANSFER_PROTOCOL_NAME.c_str());
                    immHandler.getAttribute(idsChildren[j].c_str(), &secondaryInitDest);
                    ACS_XBRM_LOG(LOG_LEVEL_INFO, "secondaryInitDest isSFTP = %d", *reinterpret_cast<int *>(secondaryInitDest.attrValues[0]) == 0 ? 1 : 0);
                    if (*reinterpret_cast<int *>(secondaryInitDest.attrValues[0]) != 0)
                    { /*0 - SFTPV2*/
                        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Invalid Transfer Protocol, errorText = %s", getExitCodeText(ACS_XBRM_UTILITY::TQ_INVALID_TRANSFER_PROTOCOL).c_str());
                        return ACS_XBRM_UTILITY::TQ_INVALID_TRANSFER_PROTOCOL;
                    }
                }
            }
            if (!primaryIDSExist)
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Invalid Transfer Protocol, errorText = %s", getExitCodeText(ACS_XBRM_UTILITY::TQ_INVALID_INITIATING_DESTINATION_SET).c_str());
                return ACS_XBRM_UTILITY::TQ_INVALID_INITIATING_DESTINATION_SET;
            }
        }
    }
    return ACS_XBRM_UTILITY::TQ_VALID;
}

ACS_CC_ReturnType ACS_XBRM_Utilities::updateReportProgress(char *objName, char *actionName, int state, int result, char *resultInfo, bool completed, char *additionalInfo, int progressPercentage, char *progressInfo, bool isRestore)
{
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "updateReportProgress::");
    ACS_CC_ImmParameter rdnAttr;
    ACS_CC_ImmParameter actionNameAttr;
    ACS_CC_ImmParameter additionalInfoAttr;
    ACS_CC_ImmParameter progressInfoAttr;
    ACS_CC_ImmParameter progressPercentageAttr;
    ACS_CC_ImmParameter resultAttr;
    ACS_CC_ImmParameter resultInfoAttr;
    ACS_CC_ImmParameter stateAttr;
    ACS_CC_ImmParameter actionIdAttr;
    ACS_CC_ImmParameter timeActionStartedAttr;
    ACS_CC_ImmParameter timeActionCompletedAttr;
    ACS_CC_ImmParameter timeOfLastStatusUpdateAttr;

    actionNameAttr.attrName = (char *)"actionName";
    actionNameAttr.attrType = ATTR_STRINGT;
    actionNameAttr.attrValuesNum = 1;
    void *actionNameValues[1] = {reinterpret_cast<void *>(actionName)};
    actionNameAttr.attrValues = actionNameValues;

    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &actionNameAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying actionName of progressReport failed");
        return ACS_CC_FAILURE;
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying actionName of progressReport successful");
    }

    additionalInfoAttr.attrName = (char *)"additionalInfo";
    additionalInfoAttr.attrType = ATTR_STRINGT;
    additionalInfoAttr.attrValuesNum = 1;
    void *additionalInfoValues[1] = {reinterpret_cast<void *>(additionalInfo)};
    additionalInfoAttr.attrValues = additionalInfoValues;

    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &additionalInfoAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying additionalInfo of progressReport failed");
        return ACS_CC_FAILURE;
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying additionalInfo of progressReport successful");
    }

    progressInfoAttr.attrName = (char *)"progressInfo";
    progressInfoAttr.attrType = ATTR_STRINGT;
    progressInfoAttr.attrValuesNum = 1;
    void *progressInfoValues[1] = {reinterpret_cast<void *>(progressInfo)};
    progressInfoAttr.attrValues = progressInfoValues;

    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &progressInfoAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying progressInfo of progressReport failed");
        return ACS_CC_FAILURE;
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying progressInfo of progressReport successful");
    }

    progressPercentageAttr.attrName = (char *)"progressPercentage";
    progressPercentageAttr.attrType = ATTR_UINT32T;
    if (progressPercentage == -1)
    {
        progressPercentageAttr.attrValuesNum = 0;
        progressPercentageAttr.attrValues = 0;
    }
    else
    {
        progressPercentageAttr.attrValuesNum = 1;
        void *progressPercentageValues[1] = {reinterpret_cast<void *>(&progressPercentage)};
        progressPercentageAttr.attrValues = progressPercentageValues;
    }

    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &progressPercentageAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying progressPercentage of progressReport failed");
        return ACS_CC_FAILURE;
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying progressPercentage of progressReport successful");
    }

    resultAttr.attrName = (char *)"result";
    resultAttr.attrType = ATTR_INT32T;
    resultAttr.attrValuesNum = 1;
    void *resultAttrValues[1] = {reinterpret_cast<void *>(&result)};
    resultAttr.attrValues = resultAttrValues;

    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &resultAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying result of progressReport failed");
        return ACS_CC_FAILURE;
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying result of progressReport successful");
    }

    resultInfoAttr.attrName = (char *)"resultInfo";
    resultInfoAttr.attrType = ATTR_STRINGT;
    resultInfoAttr.attrValuesNum = 1;
    void *resultInfoValues[1] = {reinterpret_cast<void *>(resultInfo)};
    resultInfoAttr.attrValues = resultInfoValues;

    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &resultInfoAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying resultInfo of progressReport failed");
        return ACS_CC_FAILURE;
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying resultInfo of progressReport successful");
    }

    stateAttr.attrName = (char *)"state";
    stateAttr.attrType = ATTR_INT32T;
    stateAttr.attrValuesNum = 1;
    void *stateValues[1] = {reinterpret_cast<void *>(&state)};
    stateAttr.attrValues = stateValues;

    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &stateAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying state of progressReport failed");
        return ACS_CC_FAILURE;
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying state of progressReport successful");
    }

    if (isRestore)
    {
        actionIdAttr.attrName = (char *)"actionId";
        actionIdAttr.attrType = ATTR_UINT32T;
        actionIdAttr.attrValuesNum = 0;
        actionIdAttr.attrValues = 0;
        if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &actionIdAttr))
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying actionId of progressReport failed");
            return ACS_CC_FAILURE;
        }
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying actionId of progressReport successful");
        }

        timeActionStartedAttr.attrName = (char *)"timeActionStarted";
        timeActionStartedAttr.attrType = ATTR_STRINGT;
        timeActionStartedAttr.attrValuesNum = 1;
        void *timeActionStartedValues[1] = {reinterpret_cast<void *>(const_cast<char *>(""))};
        timeActionStartedAttr.attrValues = timeActionStartedValues;
        if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &timeActionStartedAttr))
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying timeActionStarted of progressReport failed");
            return ACS_CC_FAILURE;
        }
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying timeActionStarted of progressReport successful");
        }

        timeActionCompletedAttr.attrName = (char *)"timeActionCompleted";
        timeActionCompletedAttr.attrType = ATTR_STRINGT;
        timeActionCompletedAttr.attrValuesNum = 1;
        void *timeActionCompletedValues[1] = {reinterpret_cast<void *>(const_cast<char *>(""))};
        timeActionCompletedAttr.attrValues = timeActionCompletedValues;
        if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &timeActionCompletedAttr))
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying timeActionCompleted of progressReport failed");
            return ACS_CC_FAILURE;
        }
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying timeActionCompleted of progressReport successful");
        }

        timeOfLastStatusUpdateAttr.attrName = (char *)"timeOfLastStatusUpdate";
        timeOfLastStatusUpdateAttr.attrType = ATTR_STRINGT;
        timeOfLastStatusUpdateAttr.attrValuesNum = 1;
        void *timeOfLastStatusUpdateValues[1] = {reinterpret_cast<void *>(const_cast<char *>(""))};
        timeOfLastStatusUpdateAttr.attrValues = timeOfLastStatusUpdateValues;

        if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &timeOfLastStatusUpdateAttr))
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying timeOfLastStatusUpdate of progressReport failed");
            return ACS_CC_FAILURE;
        }
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying timeOfLastStatusUpdate of progressReport successful");
        }
    }
    else
    {
        actionIdAttr.attrName = (char *)"actionId";
        actionIdAttr.attrType = ATTR_UINT32T;
        actionIdAttr.attrValuesNum = 1;
        int actionId = -1;
        if (0 == ACE_OS::strcmp("CREATE", actionName))
        {
            actionId = 1;
        }
        else if (0 == ACE_OS::strcmp("EXPORT", actionName))
        {
            actionId = 2;
        }
        void *action[1] = {reinterpret_cast<void *>(&actionId)};
        actionIdAttr.attrValues = action;
        if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &actionIdAttr))
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying actionId of progressReport failed");
            return ACS_CC_FAILURE;
        }
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying actionId of progressReport successful");
        }
        if (!completed)
        {
            timeActionStartedAttr.attrName = (char *)"timeActionStarted";
            timeActionStartedAttr.attrType = ATTR_STRINGT;
            timeActionStartedAttr.attrValuesNum = 1;
            string currentTime = getCurrentTime();
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "getCurrentTime():::%s", currentTime.c_str());
            char *timeActionStarted = const_cast<char *>(currentTime.c_str());

            void *timeActionStartedValues[1] = {reinterpret_cast<void *>(timeActionStarted)};
            timeActionStartedAttr.attrValues = timeActionStartedValues;

            if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &timeActionStartedAttr))
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying timeActionStarted of progressReport failed");
                return ACS_CC_FAILURE;
            }
            else
            {
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying timeActionStarted of progressReport successful");
            }
        }
        else
        {
            string currentTime = getCurrentTime();
            timeActionCompletedAttr.attrName = (char *)"timeActionCompleted";
            timeActionCompletedAttr.attrType = ATTR_STRINGT;
            timeActionCompletedAttr.attrValuesNum = 1;
            char *timeActionCompleted = const_cast<char *>(currentTime.c_str());
            void *timeActionCompletedValues[1] = {reinterpret_cast<void *>(timeActionCompleted)};
            timeActionCompletedAttr.attrValues = timeActionCompletedValues;

            if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &timeActionCompletedAttr))
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying timeActionCompleted of progressReport failed");
                return ACS_CC_FAILURE;
            }
            else
            {
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying timeActionCompleted of progressReport successful");
            }

            timeOfLastStatusUpdateAttr.attrName = (char *)"timeOfLastStatusUpdate";
            timeOfLastStatusUpdateAttr.attrType = ATTR_STRINGT;
            timeOfLastStatusUpdateAttr.attrValuesNum = 1;
            char *timeOfLastStatusUpdate = const_cast<char *>(currentTime.c_str());
            void *timeOfLastStatusUpdateValues[1] = {reinterpret_cast<void *>(timeOfLastStatusUpdate)};
            timeOfLastStatusUpdateAttr.attrValues = timeOfLastStatusUpdateValues;

            if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &timeOfLastStatusUpdateAttr))
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying timeOfLastStatusUpdate of progressReport failed");
                return ACS_CC_FAILURE;
            }
            else
            {
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying timeOfLastStatusUpdate of progressReport successful");
            }
        }
    }

    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_XBRM_Utilities::resetReportProgress(char *objName)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "In reset progress");
    ACS_CC_ImmParameter actionNameAttr;
    ACS_CC_ImmParameter additionalInfoAttr;
    ACS_CC_ImmParameter progressInfoAttr;
    ACS_CC_ImmParameter progressPercentageAttr;
    ACS_CC_ImmParameter resultAttr;
    ACS_CC_ImmParameter resultInfoAttr;
    ACS_CC_ImmParameter stateAttr;
    ACS_CC_ImmParameter actionIdAttr;
    ACS_CC_ImmParameter timeActionStartedAttr;
    ACS_CC_ImmParameter timeActionCompletedAttr;
    ACS_CC_ImmParameter timeOfLastStatusUpdateAttr;

    actionNameAttr.attrName = (char *)"actionName";
    actionNameAttr.attrType = ATTR_STRINGT;
    actionNameAttr.attrValuesNum = 0;
    actionNameAttr.attrValues = 0;
    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &actionNameAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", actionNameAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", actionNameAttr.attrName);
    }

    additionalInfoAttr.attrName = (char *)"additionalInfo";
    additionalInfoAttr.attrType = ATTR_STRINGT;
    additionalInfoAttr.attrValuesNum = 1;
    char *additionalInfo = (char *)"";
    void *additionalInfoValues[1] = {reinterpret_cast<void *>(additionalInfo)};
    additionalInfoAttr.attrValues = additionalInfoValues;
    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &additionalInfoAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", additionalInfoAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", additionalInfoAttr.attrName);
    }

    progressInfoAttr.attrName = (char *)"progressInfo";
    progressInfoAttr.attrType = ATTR_STRINGT;
    progressInfoAttr.attrValuesNum = 0;
    progressInfoAttr.attrValues = 0;
    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &progressInfoAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", progressInfoAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", progressInfoAttr.attrName);
    }

    progressPercentageAttr.attrName = (char *)"progressPercentage";
    progressPercentageAttr.attrType = ATTR_UINT32T;
    progressPercentageAttr.attrValuesNum = 0;
    progressPercentageAttr.attrValues = 0;
    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &progressPercentageAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", progressPercentageAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", progressPercentageAttr.attrName);
    }

    resultAttr.attrName = (char *)"result";
    resultAttr.attrType = ATTR_INT32T;
    resultAttr.attrValuesNum = 1;
    int NOT_AVAILABLE = 3;
    void *resultAttrValues[1] = {reinterpret_cast<void *>(&NOT_AVAILABLE)};
    resultAttr.attrValues = resultAttrValues;
    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &resultAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", resultAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", resultAttr.attrName);
    }

    resultInfoAttr.attrName = (char *)"resultInfo";
    resultInfoAttr.attrType = ATTR_STRINGT;
    resultInfoAttr.attrValuesNum = 1;
    char *resultInfo = (char *)"";
    void *resultInfoValues[1] = {reinterpret_cast<void *>(resultInfo)};
    resultInfoAttr.attrValues = resultInfoValues;
    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &resultInfoAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", resultInfoAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", resultInfoAttr.attrName);
    }

    stateAttr.attrName = (char *)"state";
    stateAttr.attrType = ATTR_INT32T;
    stateAttr.attrValuesNum = 1;
    int state = 3;
    void *stateValues[1] = {reinterpret_cast<void *>(&state)};
    stateAttr.attrValues = stateValues;
    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &stateAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", stateAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", stateAttr.attrName);
    }

    actionIdAttr.attrName = (char *)"actionId";
    actionIdAttr.attrType = ATTR_UINT32T;
    actionIdAttr.attrValuesNum = 0;
    actionIdAttr.attrValues = 0;
    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &actionIdAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", actionIdAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", actionIdAttr.attrName);
    }

    timeActionStartedAttr.attrName = (char *)"timeActionStarted";
    timeActionStartedAttr.attrType = ATTR_STRINGT;
    timeActionStartedAttr.attrValuesNum = 1;
    char *timeActionStarted = (char *)"";
    void *timeActionStartedValues[1] = {reinterpret_cast<void *>(timeActionStarted)};
    timeActionStartedAttr.attrValues = timeActionStartedValues;
    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &timeActionStartedAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", timeActionStartedAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", timeActionStartedAttr.attrName);
    }

    timeActionCompletedAttr.attrName = (char *)"timeActionCompleted";
    timeActionCompletedAttr.attrType = ATTR_STRINGT;
    timeActionCompletedAttr.attrValuesNum = 1;
    char *timeActionCompleted = (char *)"";
    void *timeActionCompletedValues[1] = {reinterpret_cast<void *>(timeActionCompleted)};
    timeActionCompletedAttr.attrValues = timeActionCompletedValues;
    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &timeActionCompletedAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", timeActionCompletedAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", timeActionCompletedAttr.attrName);
    }

    timeOfLastStatusUpdateAttr.attrName = (char *)"timeOfLastStatusUpdate";
    timeOfLastStatusUpdateAttr.attrType = ATTR_STRINGT;
    timeOfLastStatusUpdateAttr.attrValuesNum = 1;
    char *timeOfLastStatusUpdate = (char *)"";
    void *timeOfLastStatusUpdateValues[1] = {reinterpret_cast<void *>(timeOfLastStatusUpdate)};
    timeOfLastStatusUpdateAttr.attrValues = timeOfLastStatusUpdateValues;

    if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &timeOfLastStatusUpdateAttr))
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", timeOfLastStatusUpdateAttr.attrName);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", timeOfLastStatusUpdateAttr.attrName);
    }

    return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType ACS_XBRM_Utilities::updateReportProgressAttribute(char *objName, char *attributeName, ACS_CC_AttrValueType attributeType, int attributeValuesNum, void *attributeValues[])
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_CC_ImmParameter reportProgress;
    reportProgress.attrName = attributeName;
    reportProgress.attrType = attributeType;
    reportProgress.attrValuesNum = attributeValuesNum;
    reportProgress.attrValues = attributeValues;
    int cnt = 0;
    while (cnt < 5)
    {
        if (ACS_CC_FAILURE == immHandler.modifyAttribute(objName, &reportProgress))
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying %s of progressReport failed", attributeName);
        }
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying %s of progressReport successful", attributeName);
            return ACS_CC_SUCCESS;
        }
        cnt++;
    }
    return ACS_CC_FAILURE;
}

string ACS_XBRM_Utilities::getExitCodeText(int exitCode)
{
    switch (exitCode)
    {
    case ACS_XBRM_UTILITY::TQ_ATTACH_FAIL:
        return "Transfer Queue Attaching Failied";
    case ACS_XBRM_UTILITY::TQ_DETACH_FAILED:
        return "Transfer Queue Detaching Failied";
    case ACS_XBRM_UTILITY::TQ_INVALID_INITIATING_DESTINATION_SET:
        return "Initiating Destination Set Not Configured in TransferQueue";
    case ACS_XBRM_UTILITY::TQ_INVALID_RESPONDING_DESTINATION_SET:
        return "Responding Destination Set Configured in TransferQueue";
    case ACS_XBRM_UTILITY::TQ_INVALID_TRANSFER_PROTOCOL:
        return "Transfer Protocol  is not SFTPV2";
    case ACS_XBRM_UTILITY::TQ_VALID:
        return "Valid Transfer Queue";

    default:
        break;
    }
    return "";
}

string ACS_XBRM_Utilities::getCurrentTime()
{
    ACS_XBRM_TRACE_FUNCTION;
    time_t t = time(0); // get time now
    struct tm *now = localtime(&t);
    char currentTime[32];
    sprintf(currentTime, "%04d-%02d-%02dT%02d:%02d:%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    std::string temp(currentTime);
    return temp;
}

string ACS_XBRM_Utilities::getCurrentTimeFormat()
{
    ACS_XBRM_TRACE_FUNCTION;
    time_t t = time(0); // get time now
    struct tm *now = localtime(&t);
    char currentTime[32];
    sprintf(currentTime, "%04d-%02d-%02dT%02d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    std::string temp(currentTime);
    return temp;
}

int ACS_XBRM_Utilities::sendFilesToGOH(string brmDir, string TQ, string fileToSend)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "sendFilesToGOH");
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "fileToSend %s", fileToSend.c_str());
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "TQ %s", TQ.c_str());
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "brmDir %s", brmDir.c_str());
    int rtn_value = 0;

    unsigned int errorCode;
    AES_OHI_FileHandler fileHandler(ACS_XBRM_UTILITY::SUBSYS.c_str(), ACS_XBRM_UTILITY::APPNAME.c_str(), TQ.c_str(), "Transfer Queue handling", brmDir.c_str());

    // Attaching to TQ
    bool exec = true;
    int count = 0;
    while (exec)
    {
        errorCode = fileHandler.attach();
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "Attaching to TQ :: errorcode %d", errorCode);
        if (errorCode == AES_OHI_NOERRORCODE)
        {
            exec = false;
        }
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Attaching to TQ failed :: retry count %d", count);
            sleep(10);
            count++;
            if (count > 2)
            {
                exec = false;
            }
        }
    }

    if (errorCode == AES_OHI_NOERRORCODE)
    {

        ACS_XBRM_LOG(LOG_LEVEL_INFO, "Succeess while attaching the transfer queue");

        AES_OHI_Filestates status;

        bool filesTransferred = false;
        bool errorInTransfer = false;

        errorCode = fileHandler.getTransferState(fileToSend.c_str(), status);
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "getTransferState :: Serrorcode %d", errorCode);

        if (status == AES_OHI_FSREADY)
        {
            //  the fileHandler->isConnected() API should check the connection verify :  but it returns
            //  always true, also when the COM connectionVerify command printouts false
            filesTransferred = true;
            errorCode = fileHandler.send(fileToSend.c_str());
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "send");
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "Serrorcode %d", errorCode);

            if (errorCode != AES_OHI_NOERRORCODE)
                errorInTransfer = true;
        }
        else
        {
            rtn_value = TQ_STATE_FAILED;
        }

        if (filesTransferred == true && errorInTransfer == true)
        {

            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Error when reporting a log file to file transfer server ");
            if (errorCode != 0)
            {
                rtn_value = TQ_SEND_FAILED;
            }
        }

        if (filesTransferred == true && errorInTransfer == false)
        {

            ACS_XBRM_LOG(LOG_LEVEL_INFO, "File successfully reported to TQ ");
        }

        errorCode = fileHandler.detach();
        if (errorCode != AES_OHI_NOERRORCODE)
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Transfer Queue send to GOH :  detach error [%d]", errorCode);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Attaching to TQ Failed ");
        if (errorCode != 0)
        {
            rtn_value = TQ_ATTACH_FAILED;
        }
    }

    return rtn_value;
}

std::string ACS_XBRM_Utilities::decryptString(const std::string &strPwdCrypt)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entered decryptString");
    const size_t log_buffer_size = 1024;
    char log_buffer[log_buffer_size] = {0};
    char *plaintext = NULL;
    std::string ciphertext = strPwdCrypt;
    std::string strPwdDecrypt("");
    SecCryptoStatus decryptStatus;

    if (!strPwdCrypt.empty())
    {
        decryptStatus = sec_crypto_decrypt_ecimpassword(&plaintext, ciphertext.c_str());
        if (decryptStatus != SEC_CRYPTO_OK)
        {
            ::snprintf(log_buffer, log_buffer_size, "Decryption failed");
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Decryoting Password Failed ");
        }
        else
        {
            strPwdDecrypt = plaintext;
        }
        if (plaintext != 0)
            free(plaintext);
    }

    return strPwdDecrypt;
}

int ACS_XBRM_Utilities::exportCpBackupToApgFileSystem(string backupFileName, string backupType, string backupRelVolume, string *errorMessage)
{
    ACS_XBRM_TRACE_FUNCTION;
    vector<ACS_APGCC_AdminOperationParamType> paramVector;
    ACS_APGCC_AdminOperationParamType paramSourceCpFile;
    ACS_APGCC_AdminOperationParamType paramDestFile;
    ACS_APGCC_AdminOperationParamType paramToZip;
    ACS_APGCC_AdminOperationParamType paramMode;
    vector<ACS_APGCC_AdminOperationParamType> outVector;
    acs_apgcc_adminoperation m_objAdmOp;
    int myOIValidationReturnValue;
    ACS_CC_ReturnType result = ACS_CC_SUCCESS;
    ACE_INT64 timeOutValue = 120 * 1000000000LL; // 120seconds
    const char *cpFileSystemDn = "AxeCpFileSystemcpFileSystemMId=1";

    char *sourceCpFile = (char *)backupRelVolume.c_str();

    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Exporting CP Backup from : %s", sourceCpFile);
    // temp dest name, TODO: to prepare file format
    char *dest = (char *)backupFileName.c_str();
    bool toZip = true;
    int mode = 0;
    char *cpName = (char *)backupType.c_str();

    char sourceCpFileAttrName[] = "sourceCpFile";
    char destAttrName[] = "dest";
    char modeAttrName[] = "mode";
    char toZipAttrName[] = "toZip";
    char cpNameAttrName[] = "cpName";

    bool isMultipleCPSystem = false;
    int admOpId;
    ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem);
    if (isMultipleCPSystem)
    {
        admOpId = 7;
        ACS_APGCC_AdminOperationParamType paramCpName;
        paramCpName.attrName = cpNameAttrName;
        paramCpName.attrType = ATTR_STRINGT;
        paramCpName.attrValues = {reinterpret_cast<void *>(cpName)};
        paramVector.push_back(paramCpName);
    }
    else
        admOpId = 2;

    paramSourceCpFile.attrName = sourceCpFileAttrName;
    paramSourceCpFile.attrType = ATTR_STRINGT;
    paramSourceCpFile.attrValues = {reinterpret_cast<void *>(sourceCpFile)};

    paramDestFile.attrName = destAttrName;
    paramDestFile.attrType = ATTR_STRINGT;
    paramDestFile.attrValues = {reinterpret_cast<void *>(dest)};

    paramMode.attrName = modeAttrName;
    paramMode.attrType = ATTR_INT32T;
    paramMode.attrValues = {reinterpret_cast<void *>(&mode)};

    paramToZip.attrName = toZipAttrName;
    paramToZip.attrType = ATTR_INT32T;
    paramToZip.attrValues = {reinterpret_cast<void *>(&toZip)};

    paramVector.push_back(paramSourceCpFile);
    paramVector.push_back(paramDestFile);
    paramVector.push_back(paramMode);
    paramVector.push_back(paramToZip);
    bool isSuccess = false;
    for (int i = 0; i <= 2 && !isSuccess; ++i)
    {
        if (i > 0)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Retry to trigger action to export CP Backup files");
            ACE_OS::sleep(5);
        }
        result = m_objAdmOp.init();
        if (result != ACS_CC_SUCCESS)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to initiate Adminperatin Handler");
            *errorMessage = "Failed to trigger action to export CP backup files to APG NBI";
            // return -1;
            continue;
        }
        // OpId for exprtCpFile is 2
        // OpId for exprtCpClusterFile is 7

        ACS_XBRM_LOG(LOG_LEVEL_INFO, "Exporting CP Backup %s to CP File System ", backupFileName.c_str());
        result = m_objAdmOp.adminOperationInvoke(cpFileSystemDn, 0, admOpId, paramVector, &myOIValidationReturnValue, timeOutValue, outVector);
        if (result != ACS_CC_SUCCESS)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Failed to trigger exportCpFile");
            *errorMessage = "Failed to trigger action to export CP backup files to APG NBI";
            m_objAdmOp.finalize();
            // return -1;
            continue;
        }
        if (myOIValidationReturnValue != 1)
        {
            string errorText_temp = "";
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "Export to CP File System thrown error");
            if(outVector.size() >= 2){
                errorText_temp = reinterpret_cast<char *>(outVector[1].attrValues);
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "ExportCpFile Error : %s", errorText_temp.c_str());
                *errorMessage = errorText_temp;
            }
            m_objAdmOp.freeadminOperationReturnValue(outVector);
            m_objAdmOp.finalize();

            continue;
            ;
        }
        else
        {
            isSuccess = true;
            m_objAdmOp.freeadminOperationReturnValue(outVector);
            m_objAdmOp.finalize();
        }
    }
    if (!isSuccess)
    {
        return -1;
    }
    else
        return 0;
}

void ACS_XBRM_Utilities::updateNextScheduledEventTime()
{
    ACS_XBRM_TRACE_FUNCTION;
    set<string> lastScheduledDateSet;

    std::vector<std::string> periodicEventObjNames;
    immHandler.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_PERIODIC_EVENT_NAME.c_str(), periodicEventObjNames);

    for (int i = 0; i < (int)periodicEventObjNames.size(); i++)
    {
        ACS_CC_ImmParameter periodicEvent;
        periodicEvent.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_LAST_SCHEDULED_DATE_NAME.c_str());
        immHandler.getAttribute(periodicEventObjNames[i].c_str(), &periodicEvent);
        string lastScheduledDate = "";
        if(periodicEvent.attrValuesNum > 0){
            lastScheduledDate = reinterpret_cast<char *>(periodicEvent.attrValues[0]);
        }
        
        ACS_CC_ImmParameter periodicEvent2;
        periodicEvent2.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_STOP_TIME_NAME.c_str());
        immHandler.getAttribute(periodicEventObjNames[i].c_str(), &periodicEvent2);
        string stopTime = "2099-12-31T23:59:59.000Z";
        if(periodicEvent2.attrValuesNum > 0){
            stopTime = reinterpret_cast<char *>(periodicEvent2.attrValues[0]);
        }
        // cout << "lastScheduledDate = " << lastScheduledDate <<endl;
        // cout << "stopTime = " << stopTime << endl;
        // cout << "compareDates(stopTime, lastScheduledDate) = " << compareDates(stopTime, lastScheduledDate) <<endl;

        if(compareDates(localZoneTime(stopTime), getCurrentTime())){
            cout << "CurrentTime is greater than stopTime" << endl;
            /* CurrentTime is greater than stopTime of Periodic Event so update the lastScheduledDate to empty */
            ACS_CC_ImmParameter periodicEvent3;
            periodicEvent3.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_LAST_SCHEDULED_DATE_NAME.c_str());
            periodicEvent3.attrType = ATTR_STRINGT;
            periodicEvent3.attrValuesNum = 1;
            void *values[1];
            values[0] = reinterpret_cast<void *>(const_cast<char *>(""));
            periodicEvent3.attrValues = values;

            if (ACS_CC_FAILURE == immHandler.modifyAttribute(periodicEventObjNames[i].c_str(), &periodicEvent3)){
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying lastScheduledDate of %s failed", periodicEventObjNames[i].c_str());
            }else{
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying lastScheduledDate of %s successful", periodicEventObjNames[i].c_str());
            }

        }else{
            /* insert lastScheduledDate value to set */
            if(0 != ACE_OS::strcmp(lastScheduledDate.c_str(), ""))
                lastScheduledDateSet.insert(lastScheduledDate.c_str());
        }


        /* if (ACS_CC_SUCCESS == immHandler.getAttribute(periodicEventObjNames[i].c_str(), &periodicEvent))
        {
            if ((periodicEvent.attrValuesNum > 0) && (0 != ACE_OS::strcmp(reinterpret_cast<char *>(periodicEvent.attrValues[0]), "")))
                lastScheduledDateSet.insert(reinterpret_cast<char *>(periodicEvent.attrValues[0]));
        } */
    }

    /* after end time lastScheduledDate should be max */

    // update nextScheduledTime in scheduler

    // need to check if the object is present or not
    std::vector<std::string> schedulerObjNames;
    immHandler.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_BACKUP_SCHEDULER_NAME.c_str(), schedulerObjNames);
    if (schedulerObjNames.size())
    {
        ACS_CC_ImmParameter scheduler;
        scheduler.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_NEXT_SCHEDULED_EVENT_TIME_NAME.c_str());
        scheduler.attrType = ATTR_STRINGT;
        scheduler.attrValuesNum = 1;
        set<std::string>::iterator it = lastScheduledDateSet.begin();
        void *values[1];
        if (lastScheduledDateSet.size())
        {
            set<std::string>::iterator it = lastScheduledDateSet.begin();
            values[0] = {reinterpret_cast<void *>(const_cast<char *>((*it).c_str()))};
        }
        else
        {
            values[0] = reinterpret_cast<void *>(const_cast<char *>(""));
        }
        scheduler.attrValues = values;
        if (ACS_CC_FAILURE == immHandler.modifyAttribute(schedulerObjNames[0].c_str(), &scheduler))
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying nextScheduledEventTime %s failed", schedulerObjNames[0].c_str());
        }
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying nextScheduledEventTime %s successful", schedulerObjNames[0].c_str());
        }
    }
}

void ACS_XBRM_Utilities::updateLastScheduledEventTime(string curScheduledTime)
{
    ACS_XBRM_TRACE_FUNCTION;
    std::vector<std::string> schedulerObjNames;
    immHandler.getClassInstances(ACS_XBRM_ImmMapper::CLASS_SYSTEM_BRM_BACKUP_SCHEDULER_NAME.c_str(), schedulerObjNames);
    if (schedulerObjNames.size())
    {
        ACS_CC_ImmParameter scheduler;
        scheduler.attrName = const_cast<char *>(ACS_XBRM_ImmMapper::ATTR_LAST_SCHEDULED_EVENT_TIME_NAME.c_str());
        scheduler.attrType = ATTR_STRINGT;
        scheduler.attrValuesNum = 1;
        void *values[1] = {reinterpret_cast<void *>(const_cast<char *>(curScheduledTime.c_str()))};
        scheduler.attrValues = values;
        if (ACS_CC_FAILURE == immHandler.modifyAttribute(schedulerObjNames[0].c_str(), &scheduler))
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "modifying lastScheduledEventTime %s failed", schedulerObjNames[0].c_str());
        }
        else
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "modifying lastScheduledEventTime %s successful", schedulerObjNames[0].c_str());
        }
    }
}

int ACS_XBRM_Utilities::getPercentage(int currentDomainNumber, int noOfDomains)
{
    /**
     * 75 for backup create
     * 25 for backup export
    */
    if (currentDomainNumber > noOfDomains)
        return 100;
    else
        return int(75 * currentDomainNumber / (1.0 * noOfDomains));
}

/*===================================================================
        ROUTINE: getImmAttribute
=================================================================== */
string ACS_XBRM_Utilities::getImmAttribute(string dnName, string attributeName)
{
    ACS_XBRM_TRACE_FUNCTION;
    char *attribute = const_cast<char *>(attributeName.c_str());
    char time[128] = {0};
    OmHandler immHandler;
    ACS_CC_ReturnType returnCode;
    ACS_CC_ImmParameter ImmParameter;
    ImmParameter.attrName = attribute;

    returnCode = immHandler.Init();
    if (returnCode != ACS_CC_SUCCESS)
    {
        cout << "Error in getting stopTime" << endl;
    }
    else
    {
        returnCode = immHandler.getAttribute(dnName.c_str(), &ImmParameter);
        if (returnCode == ACS_CC_SUCCESS)
        {
            if (ImmParameter.attrValuesNum)
                strncpy(time, reinterpret_cast<char *>(ImmParameter.attrValues[0]), sizeof(time) - 1);
        }
    }
    string Time(time);

    immHandler.Finalize();

    return Time;
}

/*===================================================================
        ROUTINE: timeValidation
=================================================================== */
ACE_INT32 ACS_XBRM_Utilities::timeValidation(string &time, string attributeType)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Entering ::timeValidation(string time, string attributeType)  ");

    ACS_XBRM_LOG(LOG_LEVEL_INFO, "time %s", time.c_str());
    int returnCode = -1;
    size_t Z_pos, plus_pos, minus_pos;
    string currentTime = getCurrentTime();

    returnCode = date_format_validation(time); // check whether format is correct or not
    if (returnCode != 0)
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", " return after invalid date format");
        return returnCode;
    }

    if (time.size() > 19)
    {
        string temp = time.substr(19, time.size() - 19);
        Z_pos = temp.find('Z');
        plus_pos = temp.find('+');
        minus_pos = temp.find('-');

        if (!(Z_pos != std::string::npos) && ((minus_pos != std::string::npos) || (plus_pos != std::string::npos)))
        {
            returnCode = timezone_validation(time);
            if (returnCode != 0)
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", "return after invalid timezone in timeValidation ");
                return ACS_XBRM_INVALID_TIMEZONE;
            }
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, " Time before zone change:::%s", time.c_str());
            time = localZoneTime(time);
        }
    }

    if (attributeType != stopTimeAttr)
    {
        // ACS_XBRM_LOG(LOG_LEVEL_INFO,"From time validation %s", time);
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", "check if start time is furure time ");
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "cuurenttime %s", currentTime.c_str());
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "time %s", time.c_str());

        // time = localZoneTime(time);
        if (compareDates(currentTime, time) == false) // condition to check provided time is a future time
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", " return after starttime expired in timeValidation");
            return ACS_XBRM_STARTTIME_EXPIRED;
        }
    }

    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", "Leaving acs_XBRMs_global_ObjectImpl::timeValidation(string time, string attributeType) ");
    return ACS_CC_SUCCESS;
}

/*===================================================================
        ROUTINE:compareDates
=================================================================== */
bool ACS_XBRM_Utilities::compareDates(string d1, string d2)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entered compareDates");

    string NearestTime = getNearestTime(d1, d2);
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "NearestTime::%s ", NearestTime.c_str());

    if (NearestTime.substr(0, 19) == d1.substr(0, 19))
        return true;

    else
        return false;
}

/*===================================================================
        ROUTINE: date_format_validation
=================================================================== */
ACE_INT32 ACS_XBRM_Utilities::date_format_validation(string startTime)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", " Entering  XBRM::date_format_validation(string startTime)");
    int dayDD, monthMM, yearYY;
    yearYY = atoi((startTime.substr(0, 4)).c_str());
    dayDD = atoi((startTime.substr(8, 2)).c_str());
    monthMM = atoi((startTime.substr(5, 2)).c_str());

    if (monthMM == 2)
    {
        // To check for number of days in leap year
        if (((yearYY % 4 == 0) && !(yearYY % 100 == 0)) || (yearYY % 400 == 0))
        {
            if ((dayDD < 1) || (dayDD > 29))
            {
                ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", " returning from  XBRM::date_format_validation  no of days");
                return ACS_XBRM_INVALID_TIME;
            }
        }
        else if ((dayDD < 1) || (dayDD > 28))
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", " return from  XBRM::date_format_validation after no of days");
            return ACS_XBRM_INVALID_TIME;
        }
    }

    if (monthMM == 1 || monthMM == 3 || monthMM == 5 || monthMM == 7 || monthMM == 8 || monthMM == 10 || monthMM == 12)
    {
        if (dayDD < 1 || dayDD > 31)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", " return from  XBRM::date_format_validation days");
            return ACS_XBRM_INVALID_TIME;
        }
    }
    else
    {
        if (dayDD < 1 || dayDD > 30)
        {
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", " return from  XBRM::date_format_validation(string startTime)");
            return ACS_XBRM_INVALID_TIME;
        }
    }

    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "%s", "Leaving  XBRM::date_format_validation(string startTime) ");
    return ACS_CC_SUCCESS;
}

/*===================================================================
        ROUTINE: timezone_validation
=================================================================== */
ACE_INT32 ACS_XBRM_Utilities::timezone_validation(string startTime)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Entering  XBRM::timezone_validation(string startTime) ");
    string sec_str = startTime.substr(startTime.size() - 2, 2);
    string sign = startTime.substr(startTime.size() - 6, 1);
    int hour_str = atoi(startTime.substr(startTime.size() - 5, 2).c_str());

    if (sec_str == "45")
    {
        if (!(sign == "+" && (hour_str == 5 || hour_str == 12 || hour_str == 8)))
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", " return from  XBRM::timezone_validation after invalid time zone ");
            return ACS_XBRM_INVALID_TIMEZONE;
        }
    }
    else if (sec_str == "30")
    {
        if (sign == "-")
        {
            if (!(hour_str == 3 || hour_str == 4 || hour_str == 9))
            {
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", " return from  XBRM::timezone_validation after time zone ");
                return ACS_XBRM_INVALID_TIMEZONE;
            }
        }
        else
        {
            if (!(hour_str == 3 || hour_str == 4 || hour_str == 5 || hour_str == 6 || hour_str == 9 || hour_str == 10 || hour_str == 11))
            {
                ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", " return from  XBRM::timezone_validation invalid time zone ");
                return ACS_XBRM_INVALID_TIMEZONE;
            }
        }
    }
    else if (sec_str == "00")
    {
        if (sign == "+" && !(hour_str >= 0 && hour_str <= 14))
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", " return from  XBRM::timezone_validation after invalid time zone ");
            return ACS_XBRM_INVALID_TIMEZONE;
        }
        else if (sign == "-" && !(hour_str >= 0 && hour_str <= 12))
        {
            ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", " return from  XBRM::timezone_validation after invalid time zone ");
            return ACS_XBRM_INVALID_TIMEZONE;
        }
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "return from  XBRM::timezone_validation after invalid time zone ");
        return ACS_XBRM_INVALID_TIMEZONE;
    }
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Leaving  XBRM::timezone_validation(string startTime) ");
    return ACS_CC_SUCCESS;
}

/*===================================================================
        ROUTINE:localZoneTime
=================================================================== */
string ACS_XBRM_Utilities::localZoneTime(string date)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "localTimeZone ");
    if (date.size() <= 19)
    {
        return date;
    }
    else
    {

        string temp = date.substr(19, date.size() - 19);
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "temp:::::::: %s", temp.c_str());
        char periodicity[32] = {0};
        size_t z_pos = temp.find('Z');
        size_t plus_pos = temp.find('+');
        int oneday = 1440; // i.e 24 hours
        int sign1, sign2, minutesDifference1, minutesDifference2;

        string localTimeZone = getLocalTimeZone(); // It would be in "(GMT+01:00) Europe/Rome" format
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "localTimeZone:::::::: %s", localTimeZone.c_str());
        if (localTimeZone != "")
        {
            size_t gmt_pos = localTimeZone.find(GMT);
            string timezoneDifference = localTimeZone.substr(gmt_pos + 3, 6); // In +01:00 format
            string diff = timezoneDifference.substr(0, 1);
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "diff:::::::: %s", diff.c_str());
            if (localTimeZone != "")
                sign1 = (diff == "+") ? 1 : -1;
            sign2 = (z_pos != std::string::npos) ? 0 : (plus_pos != std::string::npos) ? -1
                                                                                       : 1;
            minutesDifference1 = atoi((timezoneDifference.substr(1, 2)).c_str()) * 60 + atoi((timezoneDifference.substr(4, 2)).c_str());
            minutesDifference2 = 0;
        }
        else
        {
            return date;
        }
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "sign1:::::::: %d", sign1);
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "sign2:::::::: %d", sign2);
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "minutesDifference1:::::::: %d", minutesDifference1);

        if (sign2 != 0)
        {
            int hour = atoi((date.substr(date.size() - 5, 2)).c_str());
            int minutes = atoi((date.substr(date.size() - 2, 2)).c_str());
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "hpour:::::::: %d", hour);
            ACS_XBRM_LOG(LOG_LEVEL_ERROR, "min:::::::: %d", minutes);
            minutesDifference2 = (hour * 60) + minutes;
        }
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "minutesDifference2:::::::: %d", minutesDifference2);
        if ((sign1 == 1) && (sign2 == 0 || sign2 == 1))
        {
            sprintf(periodicity, "%d-%d-%dT%d:%d:%d", 0, 0, 0, 0, minutesDifference1 + minutesDifference2, 0);

            int day = atoi((date.substr(8, 2)).c_str());
            date = nextSchedule(date, periodicity, day);
        }
        else
        {
            date = decreaseOneDay(date);
            int minutesDifference = oneday + (sign1 * minutesDifference1) + (sign2 * minutesDifference2);
            sprintf(periodicity, "%d-%d-%dT%d:%d:%d", 0, 0, 0, 0, minutesDifference, 0);
            int day = atoi((date.substr(8, 2)).c_str());
            date = nextSchedule(date, periodicity, day);
        }
    }

    return date;
}
// /*===================================================================
//         ROUTINE:getLocalTimeZone
// =================================================================== */
string ACS_XBRM_Utilities::getLocalTimeZone()
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "getLocalTimeZone ");
    vector<string> vec;
    int numDef = 0;
    int returnCode;
    string timeZone = "";

    returnCode = getImmInstances(TIME_ZONE_LINK, vec);
    if (returnCode == ACS_CC_SUCCESS)
    {
        numDef = vec.size();
        for (int i = 0; i < numDef; i++)
        {
            int id = getImmIntAttribute(vec[i], timeZoneIdAttr);

            if (id == 0)
                timeZone = getImmAttribute(vec[i], timeZoneStringAttr);
        }
    }
    else
    {
        cout << "Error in getting single event instances" << endl;
    }

    return timeZone;
}
/*==================================================================
        ROUTINE:getImmInstances
=================================================================== */
int ACS_XBRM_Utilities::getImmInstances(const char *className, vector<string> &dnList)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "getImmInstances ");

    OmHandler immHandler;
    ACS_CC_ReturnType returnCode;
    int errorCode;

    returnCode = immHandler.Init();
    if (returnCode != ACS_CC_SUCCESS)
    {
        return ACS_CC_FAILURE;
    }

    returnCode = immHandler.getClassInstances(className, dnList);
    if (returnCode != ACS_CC_SUCCESS)
    {
        errorCode = immHandler.getInternalLastError();
        immHandler.Finalize();
        return errorCode;
    }
    immHandler.Finalize();

    return ACS_CC_SUCCESS;
}
string ACS_XBRM_Utilities::getNearestTime(string d1, string d2)
{
    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entered getNearestTime");
    int year1, mon1, day1, hh1, mm1, ss1;
    int year2, mon2, day2, hh2, mm2, ss2;
    int res;

    sscanf(d1.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d", &year1, &mon1, &day1, &hh1, &mm1, &ss1);
    sscanf(d2.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d", &year2, &mon2, &day2, &hh2, &mm2, &ss2);
    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "year1 %d", year1);
    ACS_XBRM_LOG(LOG_LEVEL_ERROR, "year2 %d", year2);

    res = (year1 == year2) ? BOTH : (year1 < year2 ? FRST : SCND);

    if (res == BOTH)
        res = (mon1 == mon2) ? BOTH : (mon2 < mon1 ? SCND : FRST);

    if (res == BOTH)
        res = (day1 == day2) ? BOTH : (day2 < day1 ? SCND : FRST);

    if (res == BOTH)
        res = (hh1 == hh2) ? BOTH : (hh2 < hh1 ? SCND : FRST);

    if (res == BOTH)
        res = (mm1 == mm2) ? BOTH : (mm2 < mm1 ? SCND : FRST);

    if (res == BOTH)
        res = ss1 == ss2 ? BOTH : (ss2 < ss1 ? SCND : FRST);

    if (res == FRST)
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "in if");
        return (d1);
    }
    else
    {
        ACS_XBRM_LOG(LOG_LEVEL_ERROR, "in else");
        return (d2);
    }
}
string ACS_XBRM_Utilities::nextSchedule(string d1, string d2, int day)
{

    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entered nextSchedule");

    DT dt1;
    TM t1;
    char nextDate[100];
    string nxtDay;

    sscanf(d1.c_str(), "%4d-%2d-%2dT%2d%*c%2d%*c%2d", &dt1.yy, &dt1.mon, &dt1.dd, &dt1.hh, &dt1.mm, &dt1.ss);
    sscanf(d2.c_str(), "%d-%d-%dT%d:%d:%d", &t1.mon, &t1.wk, &t1.dd, &t1.hh, &t1.mm, &t1.ss);

    dt1.yy = dt1.yy + (t1.mon / 12);
    dt1.mon = dt1.mon + (t1.mon % 12);
    if (dt1.mon > 12)
    {
        dt1.mon = dt1.mon % 12;
        dt1.yy++;
    }

    /*This if block will execute only when operator provides some value for months attribute and a startdate with a value greater then 28.
    Ex : If periodicity is 1 month and starttime is jan 31 then scheduling would be as follows feb 28/29, mar 31, apr 30 ....
    To handle the above peculiar case this code snippet was written.*/
    if (t1.mon != 0)
    {
        int lastDay = lastDayOfMonth(dt1);
        switch (day)
        {
        case 31:
            dt1.dd = lastDay;
            break;

        case 30:
        case 29:
            if (dt1.mon == 2)
                dt1.dd = lastDay;
            else
                dt1.dd = day;
            break;

        default:
            break;
        }
    }

    addWeeks(dt1, t1.wk);

    addDays(dt1, t1.dd);

    addTime(dt1, t1);

    if (dt1.yy > 9999)
        nxtDay = NONEXTSCHEDULE;
    else
    {
        sprintf(nextDate, "%04d-%02d-%02dT%02d:%02d:%02d", dt1.yy, dt1.mon, dt1.dd, dt1.hh, dt1.mm, dt1.ss);
        nxtDay = string(nextDate);
    }

    return (string(nxtDay));
}

/*===================================================================
                   ROUTINE: addTime
=================================================================== */
void ACS_XBRM_Utilities::addTime(DT &dt, TM t1)
{

    ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entered addTime");

    convSec(dt, t1);
    convMin(dt, t1);
    convHour(dt, t1);
}

/*===================================================================
                   ROUTINE:addWeeks
=================================================================== */
void ACS_XBRM_Utilities::addWeeks(DT &dt, int wk)
{
   
       ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entered addWeeks");
    int i = 1;
    while (i <= wk)
    {
        addDays(dt, WK);
        i++;
    }
}

/*===================================================================
                   ROUTINE:addDays
=================================================================== */
void ACS_XBRM_Utilities::addDays(DT &dt, int days)
{
       ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entered addDays"); 
    DT dt2;
    int i = 1;
    while (i <= days)
    {
        nextDay(dt, dt2);

        dt.yy = dt2.yy; /* Copy dt2 to dt */
        dt.mon = dt2.mon;
        dt.dd = dt2.dd;
        i++;
    }
}

/*===================================================================
                   ROUTINE:convHour
=================================================================== */
void ACS_XBRM_Utilities::convHour(DT &dt, TM t1)
{
       ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entered convHour"); 

    DT dt1;

    dt1.yy = 0;
    dt1.mon = 0;
    dt1.dd = 0;
    dt1.hh = 0;
    dt1.mm = 0;
    dt1.ss = 0;

    dt1.dd = t1.hh / 24;
    dt1.hh = t1.hh % 24;

    addDates(dt, dt1);
}

/*===================================================================
                   ROUTINE:convMin
=================================================================== */
void ACS_XBRM_Utilities::convMin(DT &dt, TM t1)
{
     ACS_XBRM_TRACE_FUNCTION;
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entered convMin"); 
    DT dt1;

    dt1.yy = 0;
    dt1.mon = 0;
    dt1.dd = 0;
    dt1.hh = 0;
    dt1.mm = 0;
    dt1.ss = 0;

    dt1.mm = t1.mm % 60;
    dt1.hh = t1.mm / 60;

    dt1.dd = dt1.hh / 24;
    dt1.hh = dt1.hh % 24;

    addDates(dt, dt1);
}

/*===================================================================
                   ROUTINE:convSec
=================================================================== */
void ACS_XBRM_Utilities::convSec(DT &dt, TM t1)
{

    DT dt1;

    dt1.yy = 0;
    dt1.mon = 0;
    dt1.dd = 0;
    dt1.hh = 0;
    dt1.mm = 0;
    dt1.ss = 0;

    dt1.ss = t1.ss % 60;
    dt1.mm = dt1.mm + (t1.ss / 60);

    dt1.hh = dt1.mm / 60;
    dt1.mm = dt1.mm % 60;

    dt1.dd = dt1.hh / 24;
    dt1.hh = dt1.dd % 24;

    addDates(dt, dt1);
}

/*===================================================================
                   ROUTINE: addDates
=================================================================== */
void ACS_XBRM_Utilities::addDates(DT &dt, DT dt1)
{

    int dd;
    int hh;
    int mm;
    int ss;

    ss = dt.ss + dt1.ss;
    dt.ss = ss % 60;
    dt.mm = dt.mm + (ss / 60);

    mm = dt.mm + dt1.mm;

    dt.mm = mm % 60;
    dt.hh = dt.hh + (mm / 60);

    hh = dt.hh + dt1.hh;

    dt.hh = hh % 24;
    dd = hh / 24;

    dd = dd + dt1.dd;

    addDays(dt, dd);
}

/*===================================================================
                   ROUTINE:lastDayOfMonth
=================================================================== */
int ACS_XBRM_Utilities::lastDayOfMonth(DT &dt)
{

    int lDay;

    lDay = lastDay[dt.mon - 1];

    if (dt.mon == FEB && isLeapYear(dt))
        lDay = 29;

    return (lDay);
}

/*===================================================================
                   ROUTINE:isLeapYear
=================================================================== */
bool ACS_XBRM_Utilities::isLeapYear(DT dt)
{

    if ((dt.yy % 4 == 0 && dt.yy % 100 != 0) || (dt.yy % 400 == 0))
        return true;
    return (false);
}

/*===================================================================
        ROUTINE:decreaseOneDay
=================================================================== */
string ACS_XBRM_Utilities::decreaseOneDay(string date)
{

    char temp[32] = {0};
    int yy, mm, day;
    string time = date.substr(11, date.size() - 1);
    yy = atoi((date.substr(0, 4)).c_str());
    mm = atoi((date.substr(5, 2)).c_str());
    day = atoi((date.substr(8, 2)).c_str());

    if (day != 1)
        day = day - 1;
    else
    {
        if (mm != 1)
        {
            mm = mm - 1;
            if (isLeapYear(yy) == true && mm == 2)
                day = 29;
            else
                day = lastDay[mm - 1];
        }
        else
        {
            yy = yy - 1;
            mm = 12;
            day = 31;
        }
    }
    sprintf(temp, "%04d-%02d-%02dT%s", yy, mm, day, time.c_str());
    string modifiedDate(temp);

    return modifiedDate;
}

/*===================================================================
        ROUTINE:isLeapYear
=================================================================== */
bool ACS_XBRM_Utilities::isLeapYear(int year)
{
    // No debug here because it overflow the log file
    if (((year % 4 == 0) && !(year % 100 == 0)) || (year % 400 == 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*===================================================================
                   ROUTINE: nextDay
=================================================================== */
void ACS_XBRM_Utilities::nextDay(DT crntDay, DT &nxtDay)
{
    nxtDay.dd = crntDay.dd + 1;
    nxtDay.mon = crntDay.mon;
    nxtDay.yy = crntDay.yy;

    if (nxtDay.dd > lastDay[nxtDay.mon - 1])
    {
        if (nxtDay.mon == FEB)
        {
            if (isLeapYear(nxtDay) && nxtDay.dd == lastDay[nxtDay.mon - 1] + 1)
            {
                nxtDay.dd = 29;
            }
            else
            {
                nxtDay.dd = 1;
                nxtDay.mon = 3;
            }
        }
        else
        {
            nxtDay.dd = 1;

            if (nxtDay.mon == DEC)
            {
                nxtDay.mon = JAN;
                nxtDay.yy++;
            }
            else
            {
                nxtDay.mon++;
            }
        }
    }
}

/*===================================================================
        ROUTINE: getImmIntAttribute
=================================================================== */
unsigned int ACS_XBRM_Utilities::getImmIntAttribute(string dnName, string attributeName)
{
    char *attribute = const_cast<char *>(attributeName.c_str());
    OmHandler immHandler;
    ACS_CC_ReturnType returnCode;
    ACS_CC_ImmParameter ImmParameter;
    ImmParameter.attrName = attribute;
    int value = 0;

    returnCode = immHandler.Init();
    if (returnCode != ACS_CC_SUCCESS)
    {
        cout << "Error in getting stopTime" << endl;
    }
    else
    {
        returnCode = immHandler.getAttribute(dnName.c_str(), &ImmParameter);
        if (returnCode == ACS_CC_SUCCESS)
        {
            value = *(reinterpret_cast<int *>(ImmParameter.attrValues[0]));
        }
    }

    immHandler.Finalize();
    return value;
}

/*===================================================================
        ROUTINE:isAdminStateLOCKED
=================================================================== */
bool ACS_XBRM_Utilities::isAdminStateLOCKED(string rdn)
{

    size_t pos = rdn.find_first_of(',');
    rdn = rdn.substr(pos + 1, rdn.size() - 1);
    int state = getImmIntAttribute(rdn, adminStateAttr);
    if (state)
        return false;

    return true;
}
/*===================================================================
        ROUTINE: updateNextScheduledTime
=================================================================== */
void ACS_XBRM_Utilities::updateNextScheduledTime(string eventRDN, vector<eventDetails> events, int eventType, int changeType)
{
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "Entering ACS_XBRM_Utilities::updateNextScheduledTime(string eventRDN) ");
    size_t pos = eventRDN.find_first_of(',');
    string nextScheduleTime;
    string jobSchedulerRDN = eventRDN.substr(pos + 1, eventRDN.size());

    eventRDN = parseRDN(eventRDN) + (string)COMMA_STR;

    ACS_CC_ReturnType returnCode;
    set<std::string> scheduledTimes;

    int size = events.size();

    if (size != 0 && changeType == CREATE_T)
    {

        ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "updateNextScheduledTime called after event creation");
        for (int i = 0; i < size; i++)
        {
            if (events[i].scheduleDate != NONEXTSCHEDULE)
                scheduledTimes.insert(events[i].scheduleDate);
        }

        getSchedulesOfPeriodicEvents(scheduledTimes, eventRDN);
    }
    else if (size != 0 && changeType == MODIFY)
    {

        getSchedulesOfPeriodicEvents(scheduledTimes, eventRDN, events, eventType);
    }
    else if (size == 0)
    {

        getSchedulesOfPeriodicEvents(scheduledTimes, eventRDN);
    }

    if (scheduledTimes.size() != 0)
    {
        set<string>::iterator it = scheduledTimes.begin();
        nextScheduleTime = *it;
        char *temp = const_cast<char *>(nextScheduleTime.c_str());
        void *value[1] = {reinterpret_cast<void *>(temp)};

        for (int i = 0; i < 20; i++)
        {
            returnCode = updateImmAttribute(jobSchedulerRDN, nextScheduledTimeAttr, ATTR_STRINGT, value);
            if (returnCode == ACS_CC_FAILURE)
            {
                //retry
            }
            else
                break;
        }
    }

    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Leaving acs_hcs_global_ObjectImpl::updateNextScheduledTime(string eventRDN) ");
}

/*===================================================================
        ROUTINE:getSchedulesOfPeriodicEvents
=================================================================== */
void ACS_XBRM_Utilities::getSchedulesOfPeriodicEvents(set<string> &scheduledTimes, string eventRDN, vector<eventDetails> events, int eventType)
{
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Entering  ACS_XBRM_Utilities::getSchedulesOfPeriodicEvents ");
    std::vector<std::string> pd_dnList_periodic;
    int numDef = 0;
    int returnCode;

    returnCode = getImmInstances(CLASS_BRM_PERIODIC_EVENT_NAME, pd_dnList_periodic);
    if (returnCode == ACS_CC_SUCCESS)
    {
        int events_size = events.size();
        if (eventType == PERIODIC)
        {
            for (int i = 0; i < events_size; i++)
            {
                string temp = events[i].event_rdn;
                int size = pd_dnList_periodic.size();
                for (int j = 0; j < size; j++)
                {
                    size_t Pos = pd_dnList_periodic[j].find(temp);
                    if (Pos != std::string::npos)
                    {
                        pd_dnList_periodic.erase(pd_dnList_periodic.begin() + j);
                        scheduledTimes.insert(events[i].scheduleDate);
                        break;
                    }
                }
            }
        }

        numDef = pd_dnList_periodic.size();

        for (int i = 0; i < numDef; i++)
        {

            size_t dnPos = pd_dnList_periodic[i].find(eventRDN);
            if (dnPos != std::string::npos)
            {
                string scheduledTime = getImmAttribute(pd_dnList_periodic[i], startTimeAttr);

                if (scheduledTime != NONEXTSCHEDULE && scheduledTime != "") // To check whether event reached stop time
                    scheduledTimes.insert(scheduledTime);
            }
            else
                return;
        }
    }
    else
    {
        cout << "Error in getting periodic event instances" << endl;
    }
    ACS_XBRM_LOG(LOG_LEVEL_INFO, "%s", "Leaving  acs_hcs_global_ObjectImpl::getSchedulesOfPeriodicEvents ");
}

/*===================================================================
        ROUTINE: parseRDN
=================================================================== */
string ACS_XBRM_Utilities::parseRDN(string eventRDN)
{

    size_t pos = eventRDN.find_last_of(',');
    eventRDN = eventRDN.substr(0, pos);
    pos = eventRDN.find_last_of(',');
    eventRDN = eventRDN.substr(pos + 1, eventRDN.size() - pos);

    return eventRDN;
}

/*===================================================================
        ROUTINE: updateImmAttribute
=================================================================== */
ACS_CC_ReturnType ACS_XBRM_Utilities::updateImmAttribute(string rdn, string attr, ACS_CC_AttrValueType type, void **value)
{

    char *attribute = const_cast<char *>(attr.c_str());
    ACS_CC_ReturnType returnCode;
    OmHandler immHandler;
    ACS_CC_ImmParameter ImmParameter;
    ImmParameter.attrName = attribute;
    ImmParameter.attrType = type;
    ImmParameter.attrValuesNum = 1;
    ImmParameter.attrValues = value;

    returnCode = immHandler.Init();
    if (returnCode != ACS_CC_SUCCESS)
    {
        cout << "ERROR: init FAILURE!!!\n";
    }
    returnCode = immHandler.modifyAttribute(rdn.c_str(), &ImmParameter);
    if (returnCode == ACS_CC_FAILURE)
    {
    }
    else
    {
    }

    immHandler.Finalize();

    return returnCode;
}
/*===================================================================
        ROUTINE: periodic_updationAfterJobExecution
=================================================================== */
void ACS_XBRM_Utilities::periodic_updationAfterJobExecution(string rdn)
{

    char *objName = const_cast<char *>(rdn.c_str());

    OmHandler immHandler;
    ACS_CC_ReturnType returnCode;
    vector<ACS_APGCC_ImmAttribute *> attributes;

    char startTime[32] = {0};
    char lastScheduleDate[32] = {0};
    char stopTime[32] = {0};
    int days_value = 0;
    int hours_value = 0;
    int weeks_value = 0;
    int months_value = 0;
    int minutes_value = 0;
    char periodicity[32] = {0};
    int day = 0;
    int month = 0;
    int year = 0;

    returnCode = immHandler.Init();
    if (returnCode != ACS_CC_SUCCESS)
    {
        cout << "ERROR: init FAILURE!!!\n";
    }

    ACS_APGCC_ImmAttribute LastScheduleDate;
    ACS_APGCC_ImmAttribute StartTime;
    ACS_APGCC_ImmAttribute StopTime;
    ACS_APGCC_ImmAttribute Months;
    ACS_APGCC_ImmAttribute Weeks;
    ACS_APGCC_ImmAttribute Hours;
    ACS_APGCC_ImmAttribute Days;
    ACS_APGCC_ImmAttribute Minutes;

    StartTime.attrName = startTimeAttr;
    LastScheduleDate.attrName = LASTSCHEDULEDATE;
    StopTime.attrName = stopTimeAttr;
    Months.attrName = monthsAttr;
    Days.attrName = daysAttr;
    Hours.attrName = hoursAttr;
    Weeks.attrName = weeksAttr;
    Minutes.attrName = minutesAttr;

    attributes.push_back(&LastScheduleDate);
    attributes.push_back(&StopTime);
    attributes.push_back(&Months);
    attributes.push_back(&Minutes);
    attributes.push_back(&Hours);
    attributes.push_back(&Weeks);
    attributes.push_back(&Days);
    attributes.push_back(&StartTime);

    returnCode = immHandler.getAttribute(objName, attributes);
    if (returnCode == ACS_CC_FAILURE)
    {
        cout << "getInternalLastError :" << immHandler.getInternalLastError() << endl;
        cout << "getInternalLastErrorText :" << immHandler.getInternalLastErrorText() << endl;
    }
    else
    {
        strncpy(lastScheduleDate, (reinterpret_cast<char *>(attributes[0]->attrValues[0])), sizeof(lastScheduleDate) - 1);
        strncpy(stopTime, (reinterpret_cast<char *>(attributes[1]->attrValues[0])), sizeof(stopTime) - 1);
        months_value = *(reinterpret_cast<int *>(attributes[2]->attrValues[0]));
        weeks_value = *(reinterpret_cast<int *>(attributes[5]->attrValues[0]));
        minutes_value = *(reinterpret_cast<int *>(attributes[3]->attrValues[0]));
        days_value = *(reinterpret_cast<int *>(attributes[6]->attrValues[0]));
        hours_value = *(reinterpret_cast<int *>(attributes[4]->attrValues[0]));
        strncpy(startTime, (reinterpret_cast<char *>(attributes[7]->attrValues[0])), sizeof(startTime) - 1);
    }
    immHandler.Finalize();

    string nextExecutionDate = "";
    if ((months_value + weeks_value + days_value + hours_value + minutes_value) == 0)
        nextExecutionDate = NONEXTSCHEDULE;

    else
    {
        sprintf(periodicity, "%d-%d-%dT%d:%d:%d", months_value, weeks_value, days_value, hours_value, minutes_value, 0);
        sscanf(startTime, "%d-%d-%dT", &year, &month, &day);

        nextExecutionDate = nextSchedule(lastScheduleDate, periodicity, day);
        string currentTime = getCurrentTime();
        while (compareDates(nextExecutionDate, currentTime)){
            nextExecutionDate = nextSchedule(nextExecutionDate, periodicity, day);
        }

        string time = localZoneTime(stopTime);
        if (compareDates(time, nextExecutionDate))
            nextExecutionDate = "";
    }
    ACS_XBRM_LOG(LOG_LEVEL_WARN, "Upcoming scheduled event for periodic event %s is %s", rdn.c_str(), nextExecutionDate.c_str());

    char *value = const_cast<char *>(nextExecutionDate.c_str());
    void *value1[1] = {reinterpret_cast<char *>(value)};
    updateImmAttribute(rdn, LASTSCHEDULEDATE, ATTR_STRINGT, value1);
}