/*
 * acs_apbm_subscriber.h
 *
 *  Created on: Sep 8, 2011
 *      Author: xgiufer
 */

#ifndef ACS_APBM_SUBSCRIBER_H_
#define ACS_APBM_SUBSCRIBER_H_

#ifndef HEADER_GUARD_CLASS__acs_apbm_subscriber
#define HEADER_GUARD_CLASS__acs_apbm_subscriber acs_apbm_subscriber




#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__acs_apbm_subscriber



class __CLASS_NAME__ {

private:
	/** @brief acs_apbm_subscriber Default constructor
	 *
	 *	acs_apbm_subscriber Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ () {  } //INIZIALIZZAZIONE CON PARAMETRI UTILI AL SERVER

	/** @brief acs_apbm_subscriber Copy constructor
	 *
	 *	acs_apbm_subscriber Copy Constructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline __CLASS_NAME__ (const __CLASS_NAME__ &) {
		//INSERISCI LOG
	}


	//============//
	// Destructor //
	//============//
public:
	/** @brief acs_apbm_subscriber Destructor
	 *
	 *	acs_apbm_subscriber Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline virtual ~__CLASS_NAME__ () {
		//INSERISCI LOG
	}

private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);

public:

	//SET / GET bitmap-handle-pid
	inline void set_bitmap(int bitmap) {this->_bitmap = bitmap; }
	inline void set_handle(acs_apbm::trap_handle_t handle) {this->_handle = handle; }
	inline void set_pid(acs_apbm::trap_handle_t pid) { this->_pid = pid; }

	inline int set_bitmap() const { return this->_bitmap; }
	inline acs_apbm::trap_handle_t get_handle() const { return this->_handle; }
	inline pid_t get_pid() const{ return this->_pid; }

private:

	int _bitmap;
	acs_apbm::trap_handle_t _handle;
	pid_t _pid;

};



#endif /* ACS_APBM_SUBSCRIBER_H_ */
