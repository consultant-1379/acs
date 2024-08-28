#ifndef ACS_CS_TableRequest_h_
#define ACS_CS_TableRequest_h_ 1

#include <vector>
#include <stdint.h>

// forward declarations
class ACS_CS_Attribute;


class ACS_CS_TableRequest
{
    public:
        typedef enum {
            TableHWC_BoardAdded,
            TableHWC_BoardRemoved,
            TableNE_ModifyBoard,
            TableCP_ModifyCP
        } ACS_CS_TableRequestType;

        typedef std::vector<ACS_CS_Attribute*> AttributeVectorType;

    public:
        ACS_CS_TableRequestType getRequestType() const;
        virtual uint16_t getBoardId();

    protected:
        ACS_CS_TableRequest(ACS_CS_TableRequestType type, uint16_t boardId);

    private:
        ACS_CS_TableRequest() {}

        uint16_t boardId_;
        ACS_CS_TableRequestType requestType_;
};


class ACS_CS_HWCTableAddBoardRequest : public ACS_CS_TableRequest
{
    public:
        ACS_CS_HWCTableAddBoardRequest(uint16_t boardId, AttributeVectorType *attributes);
        AttributeVectorType getAttributes() const;

    private:
        AttributeVectorType attributes_;
};


class ACS_CS_HWCTableRemoveBoardRequest : public ACS_CS_TableRequest
{
    public:
        ACS_CS_HWCTableRemoveBoardRequest(uint16_t boardId);
};


class ACS_CS_NETableModifyBoardRequest : public ACS_CS_TableRequest
{
    public:
        ACS_CS_NETableModifyBoardRequest(uint16_t boardId, AttributeVectorType *attributes);
        AttributeVectorType getAttributes() const;

    private:
        AttributeVectorType attributes_;
};

class ACS_CS_CPIdTableModifyCPRequest : public ACS_CS_TableRequest
{
    public:
		ACS_CS_CPIdTableModifyCPRequest(uint16_t boardId, AttributeVectorType *attributes);
        AttributeVectorType getAttributes() const;

    private:
        AttributeVectorType attributes_;
};

#endif // ACS_CS_TableRequest_h_
