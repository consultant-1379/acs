#ifndef ACS_CS_API_ClientObserver_h
#define ACS_CS_API_ClientObserver_h 1

#include "memory" // for auto_ptr

template <typename ObserveeT>
class ACS_CS_API_ClientObserver 
{

  public:
      virtual ~ACS_CS_API_ClientObserver();

      virtual void update (const ObserveeT& observee) = 0;
};


template <typename ObserveeT>
inline ACS_CS_API_ClientObserver<ObserveeT>::~ACS_CS_API_ClientObserver()
{}


#endif
