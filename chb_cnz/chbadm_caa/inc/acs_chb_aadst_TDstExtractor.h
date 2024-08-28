#include "time.h"
#include <string>
#include <list>
#include "acs_chb_aadst_TDstTypes.h"

// Class contains external extract method
class TDstExtractor
{
	public:
	////////////////////////////////////////////// Costructor/Distructor
		TDstExtractor();
		virtual ~TDstExtractor();
	////////////////////////////////////////////// Public method
		bool getDstFromZone(const std::string &strTimeZone, int iStartYear, int iEndYear, TZdumpData **pZdumpObj);
	////////////////////////////////////////////// Private method
	private:
		// Clear internal list
		void Clear();
	////////////////////////////////////////////// Private Attributes
	private:
		//std::list<TZdumpItem*> m_lstItem;
		TZdumpData objZdumpData;
};
