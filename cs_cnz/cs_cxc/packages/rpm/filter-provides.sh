#!/bin/sh

FINDPROV=/usr/lib/rpm/find-provides

export PATH=/vobs/cm4ap/ntacs/cs_cnz/cs_cxc/packages/rpm:$PATH

$FINDPROV $*
