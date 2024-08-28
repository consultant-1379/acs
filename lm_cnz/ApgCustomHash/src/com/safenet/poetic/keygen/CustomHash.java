package com.safenet.poetic.keygen;

public interface CustomHash {
	long getCustomHashedLockingId(String lockingIdToHash) throws Exception;

}
