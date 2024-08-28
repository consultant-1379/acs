// ACS_CS_ReaderWriterLock.cpp
// © Copyright Ericsson AB 2007. All rights reserved.

#include "ACS_CS_ReaderWriterLock.h"

ACS_CS_ReaderWriterLock::ACS_CS_ReaderWriterLock()
{
}

ACS_CS_ReaderWriterLock::~ACS_CS_ReaderWriterLock()
{
}

void ACS_CS_ReaderWriterLock::start_reading()
{
    lock_.acquire_read();
}

void ACS_CS_ReaderWriterLock::stop_reading()
{
    lock_.release();
}

void ACS_CS_ReaderWriterLock::start_writing()
{
    lock_.acquire_write();
}

void ACS_CS_ReaderWriterLock::stop_writing()
{
    lock_.release();
}
