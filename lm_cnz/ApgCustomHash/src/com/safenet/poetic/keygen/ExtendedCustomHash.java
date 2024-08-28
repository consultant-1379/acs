package com.safenet.poetic.keygen;

public interface ExtendedCustomHash extends CustomHash {
	String getExtendedCustomHashedLockingId(String lockingIdToHash) throws Exception;
}
