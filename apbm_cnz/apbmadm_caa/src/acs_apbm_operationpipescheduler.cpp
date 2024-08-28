#include <algorithm>
#include <new>

#include <ace/TP_Reactor.h>
#include <ace/Reactor.h>
#include <ace/Guard_T.h>

#include "acs_apbm_macrosconstants.h"
#include "acs_apbm_logger.h"
#include "acs_apbm_serverworkingset.h"

#include "acs_apbm_operationpipescheduler.h"


int __CLASS_NAME__::handle_timeout (const ACE_Time_Value & /*current_time*/, const void * /*act*/) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	// Find the set of operations to be scheduled now
	tick_t tick_to_schedule = _tick_counter++;

	tick_operations_map_t::iterator map_item = _tick_operations.find(tick_to_schedule);
	operation_vector_pointer_t operation_vector_ptr = (map_item == _tick_operations.end() ? 0 : map_item->second);

	if (operation_vector_ptr) {
		// There are some operations to reschedule
		ACS_APBM_LOG(LOG_LEVEL_DEBUG,
				"An operation vector (%p) found in the map: its operations will be rescheduled now at tick counter == %u: removing the vector from the map",
				operation_vector_ptr, tick_to_schedule);
		const size_t erase_result = _tick_operations.erase(tick_to_schedule);

		ACS_APBM_LOG(erase_result ? LOG_LEVEL_DEBUG : LOG_LEVEL_ERROR,
				erase_result ? "Operation vector %p successfully removed from map and its operation are ready to be rescheduled"
										 : "Call 'erase' returned 0 (zero): operation vector %p was not removed from internal map: very strange and dangerous error",
				operation_vector_ptr);

		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "There are %zu operations to reschedule now", operation_vector_ptr->size());

		for (operation_vector_t::iterator it = operation_vector_ptr->begin(); it != operation_vector_ptr->end(); ++it) {
			operation_vector_t::value_type op = *it;
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Rescheduling operation %d...", op);

			int schedule_result = _server_working_set->operation_send(op);

			ACS_APBM_LOG(schedule_result ? LOG_LEVEL_ERROR : LOG_LEVEL_DEBUG,
					schedule_result ? "Call 'operation_send' failed: cannot reschedule the operation %d to retry its function"
													: "Operation %d rescheduled successfully, its function will be executed as soon as possible",
					op);
		}

		// Clearing the operation vector
		operation_vector_ptr->clear();

		// Moving the vector into the free vector storage
		if (erase_result) {
			size_t vector_size_old = _free_operation_vectors.size();
			_free_operation_vectors.push_back(operation_vector_ptr);
			size_t vector_size_new = _free_operation_vectors.size();

			if (vector_size_new > vector_size_old) { // OK: vector successfully inserted into the vector
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Vector operation object moved from map to the free storage internal vector ready to be reused later");
			} else { // ERROR: inserting the vector into the vector
				ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'push_back' failed: cannot insert the operation vector into the free storage vector: this vector will be destroyed");
				delete operation_vector_ptr;
			}
		}
	} else {
		//ACS_APBM_LOG(LOG_LEVEL_DEBUG, "At this tick counter (%u) no operations found that need to be rescheduled", tick_to_schedule);
	}

	return 0;
}

int __CLASS_NAME__::handle_close (ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/) {
	return 0;
}

int __CLASS_NAME__::start (ACE_Reactor * reactor, unsigned delay_ms, unsigned interval_ms) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Starting operation pipe scheduler (delay_ms == %u, interval_ms == %u)...", delay_ms, interval_ms);

	if (_reactor) {
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Operation pipe scheduler already started");
		return acs_apbm::ERR_NO_ERRORS;
	}

	ACE_Time_Value tv_delay(delay_ms / 1000, (delay_ms % 1000) * 1000);
	ACE_Time_Value tv_interval(interval_ms / 1000, (interval_ms % 1000) * 1000);

	int return_code = acs_apbm::ERR_NO_ERRORS;

	int call_result = reactor->schedule_timer(this, 0, tv_delay, tv_interval);

	if (call_result == -1) { // ERROR:
		return_code = acs_apbm::ERR_REACTOR_SCHEDULE_TIMER_FAILURE;
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Call 'schedule_timer' failed: cannot start the pipe operation scheduler into the reactor");
	} else { // OK: timer scheduled correctly
		_reactor = reactor;
		_tick_counter = 0;
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Operation pipe scheduler started successfully and internal tick counter reset to 0");
	}

	return return_code;
}

int __CLASS_NAME__::stop () {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	ACS_APBM_LOG(LOG_LEVEL_INFO, "Stopping operation pipe scheduler...");

	if (_reactor) {
		_reactor->cancel_timer(this); // Second parameter == 1 (DEFAULT) --> Do not call the handle_close method
		_reactor = 0;
		free_objects();
		ACS_APBM_LOG(LOG_LEVEL_INFO, "Operation pipe scheduler stopped successfully with a tick counter == %u", _tick_counter);
	} else ACS_APBM_LOG(LOG_LEVEL_INFO, "Operation pipe scheduler already stopped");

	return acs_apbm::ERR_NO_ERRORS;
}

int __CLASS_NAME__::schedule_operation (acs_apbm::ServerWorkOperationCodeConstants op, tick_t ticks_to_wait) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (!_reactor) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Reactor internal pointer is NULL: no reactor object binded to this "
				ACS_APBM_STRINGIZE(__CLASS_NAME__) " instance: cannot schedule the operation op == %d with ticks_to_wait == %u", op, ticks_to_wait);
		return acs_apbm::ERR_NULL_POINTER;
	}

	// Finding the operation vector where the operation op has to be added
	tick_t tick_counter_to_find = _tick_counter + ticks_to_wait;
	operation_vector_pointer_t operation_vector_ptr = 0;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Tick counter to find: _tick_counter + ticks_to_wait: %u + %u == %u",
			_tick_counter, ticks_to_wait, tick_counter_to_find);

	tick_operations_map_t::iterator item = _tick_operations.find(tick_counter_to_find);

	if (item == _tick_operations.end()) {
		// No operations vector found, create a new vector or reuse an old one in the free vector storage and add it to the tick/operations internal map
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "No operation vector found with tick counter == %u", tick_counter_to_find);

		// Is there an old free operation vector into the _free_operation_vectors storage area?
		if (_free_operation_vectors.size()) { // YES: there is at least a free operation vector that can be reused
			(operation_vector_ptr = _free_operation_vectors.back())->clear();
			_free_operation_vectors.pop_back();
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reusing an old free operation vector from internal storage: operation_vector_ptr == %p", operation_vector_ptr);
		} else { // NO: we have to create a new operation set
			operation_vector_ptr = new (std::nothrow) operation_vector_t;
			if (operation_vector_ptr) { // OK
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "A new operation vector created successfully: operation_vector_ptr == %p", operation_vector_ptr);
			} else { // ERROR: memory allocation problems
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Memory allocation problem for an operation vector: cannot allocate a new operation vector: the operation op == %d cannot be scheduled", op);
				return acs_apbm::ERR_MEMORY_BAD_ALLOC;
			}
		}

		// Add the operation vector to the tick/operations internal map and check the insertion operation
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Adding the operation vector to the internal map with tick counter == %u", tick_counter_to_find);
		std::pair<tick_operations_map_t::iterator, bool>
				ret = _tick_operations.insert(tick_operations_map_t::value_type(tick_counter_to_find, operation_vector_ptr));

		if (ret.second) { // OK
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operation vector successfully added to the map");
		} else {
			// ERROR: an operation vector with the same ticks_to_wait key is already present into the map.
			// This shouldn't occur anymore because above this item was not found.
			// To recover we can use this operation vector fetched from the map

			delete operation_vector_ptr;
			operation_vector_ptr = (ret.first)->second;

			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"An operation vector with the same tick counter key is already present into the map (%p)."
					"This shouldn't occur anymore because above this item was not found."
					"To recover we can use the operation vector fetched from the map and delete the previous one created or fetched from free vectors area",
					operation_vector_ptr);
		}
	} else operation_vector_ptr = item->second; // Operation vector found, already into the map.

	// Check if the operation op is already inserted into the operation vector
	if (std::find(operation_vector_ptr->begin(), operation_vector_ptr->end(), op) == operation_vector_ptr->end()) { // NO
		// The op operation was not found into the vector so it should be inserted
		operation_vector_ptr->push_back(op);
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operation op == %d successfully inserted and ready to be scheduled at tick counter == %u",
				op, tick_counter_to_find);
	} else { // YES
		// The op operation is already into the vector
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "The operation op == %d is already present into the operation vector", op);
                return acs_apbm::ERR_TRAP_NOT_SCHEDULED;
	}

	return acs_apbm::ERR_NO_ERRORS;
}

void __CLASS_NAME__::free_objects () {
	for (size_t i = 0; i < _free_operation_vectors.size(); delete _free_operation_vectors[i++]) ;
	_free_operation_vectors.clear();

	for (tick_operations_map_t::iterator it = _tick_operations.begin(); it != _tick_operations.end(); delete (it++)->second) ;
	_tick_operations.clear();
}

void __CLASS_NAME__::get_current_tick_counter(tick_t &currentTicksValue)
{
	currentTicksValue = _tick_counter;
}


#if 0
int __CLASS_NAME__::schedule_operation (acs_apbm::ServerWorkOperationCodeConstants op, tick_t ticks_to_wait) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (!_reactor) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Reactor internal pointer is NULL: no reactor object binded to this "
				ACS_APBM_STRINGIZE(__CLASS_NAME__) " instance: cannot schedule the operation op == %d with ticks_to_wait == %u", op, ticks_to_wait);
		return acs_apbm::ERR_NULL_POINTER;
	}

	// Insert the operation op into the set associated with the ticks_to_wait. Create a new set or reuse
	// a free one if this is the first operation with this ticks_to_wait timeout.

	operation_set_pointer_t operation_set_ptr = 0;

	tick_t tick_counter_to_find = _tick_counter + ticks_to_wait;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Tick counter to find: _tick_counter + ticks_to_wait: %u + %u == %u",
			_tick_counter, ticks_to_wait, tick_counter_to_find);

	tick_operations_map_t::iterator item = _tick_operations.find(tick_counter_to_find);

	if (item == _tick_operations.end()) {
		// No operations set found, create a new set or reuse an old one in the set vector and add it to the tick/operations internal map
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "No operation set found with tick counter == %u", tick_counter_to_find);

		// Is there an old free operation set into the vector _free_operation_sets?
		if (_free_operation_sets.size()) { // YES: there is at least a free operation set that can be reused
			operation_set_ptr = _free_operation_sets.back();
			_free_operation_sets.pop_back();
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reusing an old free operation set from internal storage: operation_set_ptr == %p", operation_set_ptr);
		} else { // NO: we have to create a new operation set
			operation_set_ptr = new (std::nothrow) operation_set_t;
			if (operation_set_ptr) { // OK
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "A new operation set created successfully: operation_set_ptr == %p", operation_set_ptr);
			} else { // ERROR: memory allocation problems
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Memory allocation problem for an operation_set: cannot allocate a new operation set: the operation op == %d cannot be scheduled", op);
				return acs_apbm::ERR_MEMORY_BAD_ALLOC;
			}
		}

		// Add the operation set to the tick/operations internal map and check the insertion operation
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Adding the operation set to the internal map with tick counter == %u", tick_counter_to_find);
		std::pair<tick_operations_map_t::iterator, bool>
				ret = _tick_operations.insert(tick_operations_map_t::value_type(tick_counter_to_find, operation_set_ptr));

		if (ret.second) { // OK
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operation set successfully added to the map");
		} else {
			// ERROR: an operation set with the same ticks_to_wait key is already present into the map.
			// This shouldn't occur anymore because above this item was not found.
			// To recover we can use this operation set fetched from the map

			delete operation_set_ptr;
			operation_set_ptr = ret.first->second;

			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"An operation set with the same tick counter key is already present into the map (%p)."
					"This shouldn't occur anymore because above this item was not found."
					"To recover we can use the operation set fetched from the map and delete the previous one created or fetched from vector",
					operation_set_ptr);
		}
	} else operation_set_ptr = item->second; // Operation set found, already into the map.

	// Add the operation op to the operation set and check the insertion operation to view
	// if the op operation is already present and scheduled. We only write a log
	if ((operation_set_ptr->insert(op)).second)
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operation op == %d successfully inserted to be scheduled at tick counter == %u", op, tick_counter_to_find);
	else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "The operation op == %d is already present to be scheduled", op);

	return acs_apbm::ERR_NO_ERRORS;
}
#endif


#if 0
int __CLASS_NAME__::schedule_operation (acs_apbm::ServerWorkOperationCodeConstants op, tick_t ticks_to_wait) {
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);

	if (!_reactor) {
		ACS_APBM_LOG(LOG_LEVEL_ERROR, "Reactor internal pointer is NULL: no reactor object binded to this "
				ACS_APBM_STRINGIZE(__CLASS_NAME__) " instance: cannot schedule the operation op == %d with ticks_to_wait == %u", op, ticks_to_wait);
		return acs_apbm::ERR_NULL_POINTER;
	}

	// Insert the operation op into the set associated with the ticks_to_wait. Create a new set or reuse
	// a free one if this is the first operation with this ticks_to_wait timeout.

	operation_set_pointer_t operation_set_ptr = 0;

	tick_t tick_counter_to_find = _tick_counter + ticks_to_wait;

	ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Tick counter to find: _tick_counter + ticks_to_wait: %u + %u == %u",
			_tick_counter, ticks_to_wait, tick_counter_to_find);





	tick_operations_map_t::iterator item = _tick_operations.find(ticks_to_wait);

	if (item == _tick_operations.end()) {
		// No operations set found, create a new set or reuse an old one in the set vector and add it to the tick/operations internal map
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "No operation set found with ticks_to_wait == %u", ticks_to_wait);

		// Is there an old free operation set into the vector _free_operation_sets?
		if (_free_operation_sets.size()) { // YES: there is at least a free operation set that can be reused
			operation_set_ptr = _free_operation_sets.back();
			_free_operation_sets.pop_back();
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Reusing an old free operation set from internal storage: operation_set_ptr == %p", operation_set_ptr);
		} else { // NO: we have to create a new operation set
			operation_set_ptr = new (std::nothrow) operation_set_t;
			if (operation_set_ptr) { // OK
				ACS_APBM_LOG(LOG_LEVEL_DEBUG, "A new operation set created successfully: operation_set_ptr == %p", operation_set_ptr);
			} else { // ERROR: memory allocation problems
				ACS_APBM_LOG(LOG_LEVEL_ERROR,
						"Memory allocation problem for an operation_set: cannot allocate a new operation set: the operation op == %d cannot be scheduled", op);
				return acs_apbm::ERR_MEMORY_BAD_ALLOC;
			}
		}

		// Add the operation set to the tick/operations internal map and check the insertion operation
		ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Adding the operation set to the internal map");
		std::pair<tick_operations_map_t::iterator, bool>
				ret = _tick_operations.insert(tick_operations_map_t::value_type(ticks_to_wait, operation_set_ptr));

		if (ret.second) { // OK
			ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operation set successfully added to the map");
		} else {
			// ERROR: an operation set with the same ticks_to_wait key is already present into the map.
			// This shouldn't occur anymore because above this item was not found.
			// To recover we can use this operation set fetched from the map

			delete operation_set_ptr;
			operation_set_ptr = ret.first->second;

			ACS_APBM_LOG(LOG_LEVEL_ERROR,
					"An operation set with the same ticks_to_wait key is already present into the map (%p)."
					"This shouldn't occur anymore because above this item was not found."
					"To recover we can use the operation set fetched from the map and delete the previous one created or fetched from vector",
					operation_set_ptr);
		}
	} else operation_set_ptr = item->second; // Operation set found, already into the map.

	// Add the operation op to the operation set and check the insertion operation to view
	// if the op operation is already present and scheduled. We only write a log
	if ((operation_set_ptr->insert(op)).second) ACS_APBM_LOG(LOG_LEVEL_DEBUG, "Operation op == %d successfully scheduled", op);
	else ACS_APBM_LOG(LOG_LEVEL_DEBUG, "The operation op == %d is already scheduled", op);

	return acs_apbm::ERR_NO_ERRORS;
}
#endif
