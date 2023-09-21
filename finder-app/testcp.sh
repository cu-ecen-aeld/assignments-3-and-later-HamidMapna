#!/bin/bash
OUTDIR=$1
echo "out directory is " ${OUTDIR}
echo "file dir is " $(dirname $(readlink -f "$0"))
CUR_DIR=$(dirname $(readlink -f "$0"))
echo "curr dir:" $CUR_DIR
cd ${OUTDIR}
pwd
cd  ${CUR_DIR}
#cd $"/home/hamid/advanced_linux_programming/aesd-assignments/finder-app"
pwd