

//	Copyright Ericsson AB 2007. All rights reserved.


#ifndef ACS_CS_PDU_h
#define ACS_CS_PDU_h 1



class ACS_CS_HeaderBase;
class ACS_CS_Primitive;









class ACS_CS_PDU 
{

  public:
      ACS_CS_PDU();

      ACS_CS_PDU(const ACS_CS_PDU &right);

      ACS_CS_PDU (ACS_CS_HeaderBase *header, ACS_CS_Primitive *primitive);

      virtual ~ACS_CS_PDU();

      ACS_CS_PDU & operator=(const ACS_CS_PDU &right);

      int getBuffer (char* buffer, int size) const;

      int getLength () const;

      const ACS_CS_Primitive * getPrimitive () const;

      const ACS_CS_HeaderBase * getHeader () const;

      void setHeader (ACS_CS_HeaderBase *header);

      void setPrimitive (ACS_CS_Primitive *primitive);

       ACS_CS_PDU * clone () const;

       bool dont_destroy_primitive;  /*Fix for TR HW53952.
                                       It is set true when the primitive object is not to be deleted along with pdu object */

  private:

       ACS_CS_HeaderBase *pduHeader;

       ACS_CS_Primitive *pduPrimitive;

};


// Class ACS_CS_PDU 



#endif
