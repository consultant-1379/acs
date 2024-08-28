#include "ACS_CS_TableRequest.h"
#include "ACS_CS_Attribute.h"


ACS_CS_TableRequest::ACS_CS_TableRequest(ACS_CS_TableRequestType type, uint16_t boardId)
{
    this->requestType_ = type;
    this->boardId_ = boardId;
}


ACS_CS_TableRequest::ACS_CS_TableRequestType ACS_CS_TableRequest::getRequestType() const
{
    return this->requestType_;
}


uint16_t ACS_CS_TableRequest::getBoardId()
{
    return this->boardId_;
}


//----- Add board request ----------------------------------------------------------------------------------------------

ACS_CS_HWCTableAddBoardRequest::ACS_CS_HWCTableAddBoardRequest(uint16_t boardId, AttributeVectorType *attributes) :
        ACS_CS_TableRequest(TableHWC_BoardAdded, boardId)
{
    AttributeVectorType::iterator it = attributes->begin();

    for (;it != attributes->end(); ++it)
    {
        this->attributes_.push_back(*it);
    }
}


ACS_CS_HWCTableAddBoardRequest::AttributeVectorType ACS_CS_HWCTableAddBoardRequest::getAttributes() const
{
    return this->attributes_;
}



//----- Remove board request -------------------------------------------------------------------------------------------

ACS_CS_HWCTableRemoveBoardRequest::ACS_CS_HWCTableRemoveBoardRequest(uint16_t boardId) :
        ACS_CS_TableRequest(TableHWC_BoardRemoved, boardId)
{
}


//----- Modify NE board request ----------------------------------------------------------------------------------------

ACS_CS_NETableModifyBoardRequest::ACS_CS_NETableModifyBoardRequest(uint16_t boardId, AttributeVectorType *attributes) :
        ACS_CS_TableRequest(TableNE_ModifyBoard, boardId)
{
    AttributeVectorType::iterator it = attributes->begin();

    for (;it != attributes->end(); ++it)
    {
        this->attributes_.push_back(*it);
    }
}


ACS_CS_NETableModifyBoardRequest::AttributeVectorType ACS_CS_NETableModifyBoardRequest::getAttributes() const
{
    return this->attributes_;
}


//----- Modify CP board request ----------------------------------------------------------------------------------------

ACS_CS_CPIdTableModifyCPRequest::ACS_CS_CPIdTableModifyCPRequest(uint16_t boardId, AttributeVectorType *attributes) :
        ACS_CS_TableRequest(TableCP_ModifyCP, boardId)
{
    AttributeVectorType::iterator it = attributes->begin();

    for (;it != attributes->end(); ++it)
    {
        this->attributes_.push_back(*it);
    }
}


ACS_CS_CPIdTableModifyCPRequest::AttributeVectorType ACS_CS_CPIdTableModifyCPRequest::getAttributes() const
{
    return this->attributes_;
}
