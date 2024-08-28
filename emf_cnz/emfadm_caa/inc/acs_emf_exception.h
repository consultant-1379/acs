/*=================================================================== */
/**
   @file   acs_emf_common.h

   @brief Header file for EMF module.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE             INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       	DD/MM/YYYY       XRAMMAT   Initial Release
 */
/*==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef ACS_EMF_EXCEPTION_H
#define ACS_EMF_EXCEPTION_H

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define ERRMSG_LEN      64
#define EXT_ERRMSG_LEN  256

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */

/*===================================================================*/
/**
      @brief  ACS_EMF_Exception class
 */
/*=================================================================== */
class ACS_EMF_Exception
{
	/*=====================================================================
						 PUBLIC DECLARATION SECTION
	 ==================================================================== */
public:
	/*=====================================================================
							CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**
		  @brief           ACS_EMF_Exception
							Default Constructor

		  @exception       none
	 */
	/*=================================================================== */
	ACS_EMF_Exception()
	{
		m_dwErrCode = 0;
		*m_szErrMsg = *m_szExtErrMsg = _T('\0');
	};

	/*=================================================================== */
	/**
		  @brief           ACS_EMF_Exception
							parameter Constructor

		  @param		   Code : const DWORD

		  @param 		   Msg : const _TCHAR*

		  @param		   ExtMsg : const _TCHAR*
							  Optional parameter

		  @exception       none
	 */
	/*=================================================================== */
	ACS_EMF_Exception(const DWORD Code, const _TCHAR* Msg, const _TCHAR* ExtMsg = NULL)
	{
		m_dwErrCode = Code;
		*m_szErrMsg = *m_szExtErrMsg = _T('\0');

#if (_MSC_VER >= 1400)
		(void)_tcsncpy_s(m_szErrMsg, ERRMSG_LEN, Msg, ERRMSG_LEN-1);
		m_szErrMsg[ERRMSG_LEN-1] = _T('\0');

		_tcscpy_s(m_szExtErrMsg, EXT_ERRMSG_LEN, m_szErrMsg);

		if (ExtMsg && *ExtMsg != _T('\0'))
		{
			size_t nLen = _tcslen(m_szExtErrMsg);
			for (; nLen > 0; nLen--)
			{
				if (m_szExtErrMsg[nLen-1] != _T('.') && m_szExtErrMsg[nLen-1] != _T(' '))
					break;
			}

			(void)_tcsncpy_s(m_szExtErrMsg+nLen, EXT_ERRMSG_LEN, _T(". "), 2);
			nLen += 2;

			(void)_tcsncpy_s(m_szExtErrMsg+nLen, EXT_ERRMSG_LEN, ExtMsg, EXT_ERRMSG_LEN-nLen-1);
			m_szExtErrMsg[nLen+(EXT_ERRMSG_LEN-nLen-1)] = _T('\0');
		}
#else
		(void)_tcsncpy(m_szErrMsg, Msg, ERRMSG_LEN-1);
		m_szErrMsg[ERRMSG_LEN-1] = _T('\0');

		_tcscpy(m_szExtErrMsg, m_szErrMsg);

		if (ExtMsg && *ExtMsg != _T('\0'))
		{
			size_t nLen = _tcslen(m_szExtErrMsg);
			for (; nLen > 0; nLen--)
			{
				if (m_szExtErrMsg[nLen-1] != _T('.') && m_szExtErrMsg[nLen-1] != _T(' '))
					break;
			}

			(void)_tcsncpy(m_szExtErrMsg+nLen, _T(". "), 2);
			nLen += 2;

			(void)_tcsncpy(m_szExtErrMsg+nLen, ExtMsg, EXT_ERRMSG_LEN-nLen-1);
			m_szExtErrMsg[nLen+(EXT_ERRMSG_LEN-nLen-1)] = _T('\0');
		}
#endif
	};
	/*=================================================================== */
	/**
   		  @brief           ErrCode

   		  @return		   const DWORD

   		  @exception       none
	 */
	/*=================================================================== */
	const DWORD ErrCode() const { return m_dwErrCode; };
	/*=================================================================== */
	/**
		  @brief           ErrMsg

		  @return		   const _TCHAR*

		  @exception       none
	 */
	/*=================================================================== */
	const _TCHAR* ErrMsg() const { return m_szErrMsg; };
	/*=================================================================== */
	/**
		  @brief           ExtErrMsg

		  @return		   const _TCHAR*

		  @exception       none
	 */
	/*=================================================================== */
	const _TCHAR* ExtErrMsg() const { return m_szExtErrMsg; };
	/*=====================================================================
						 PRIVATE DECLARATION SECTION
	 ==================================================================== */
private:
	/*=====================================================================
						 PRIVATE ATTRIBUTES
	 ==================================================================== */
	/*=================================================================== */
	/**
   		@brief   m_dwErrCode
	 */
	/*=================================================================== */
	DWORD  m_dwErrCode;
	/*=================================================================== */
	/**
   		@brief   m_szErrMsg
	 */
	/*=================================================================== */
	_TCHAR m_szErrMsg[ERRMSG_LEN];
	/*=================================================================== */
	/**
   		@brief   m_szExtErrMsg
	 */
	/*=================================================================== */
	_TCHAR m_szExtErrMsg[EXT_ERRMSG_LEN];
};

#endif
