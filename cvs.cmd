#!/bin/sh

. ./cvs.init
#export CVS_SERVER=''

if [ "$1" = "" ] ; then
	echo "Usage:"
	echo "      $0 <username> <stuff>	"
	exit -1
fi

MYUID="$1"
shift 1
cvs -d :ext:$MYUID@$MACHINE:$CVS_ROOTDIR $*

