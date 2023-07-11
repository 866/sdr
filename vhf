#!/bin/bash -x

FREQS="446000000"
# FREQS="147000000"
CHANNEL_BW=20000

# Determine center automatically
s=0
n=0
for f in $FREQS; do
	s=$(($s + $f))
	n=$(($n + 1))
done
STEP=2500
CENTR=$(($s / $n))
CENTR=$(($CENTR / $STEP))
CENTR=$(($CENTR * $STEP))

BW=1800000

rtl_sdr -f $CENTR -g 25 -s $BW - | ./scan_record_nfm.py $CENTR $BW $STEP $CHANNEL_BW $FREQS . -a $*
