#!/bin/sh

FINDREQ=/usr/lib/rpm/find-requires

export PATH=/vobs/cm4ap/ntacs/cs_cnz/cs_cxc/packages/rpm:$PATH

$FINDREQ $* | grep -v "OPENSAF_IMM_A"
