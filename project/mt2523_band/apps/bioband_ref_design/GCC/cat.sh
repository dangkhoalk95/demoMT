#!/bin/bash

###############################################################################
#Variables

PROJ_NAME=$1
OUTPATH=$2
#BL max size is 64K
BOOTLOADERMAXSIZE=65536
FREERTOSBIN="$OUTPATH"/"$PROJ_NAME".bin
FLASHBIN="$OUTPATH"/flash.bin
BOOTLOADERBIN="$OUTPATH"/mt2523_bootloader.bin
BOOTLOADERBIN_TMP="$OUTPATH"/bootloader_tmp.bin

#FreeRTOS partition max size is 1966080
FREERTOS_MAX_SIZE=$[2304*1024]
FREERTOSBIN_TMP="$OUTPATH"/freertos_tmp.bin

UBIN="$OUTPATH"/ubin.bin
###############################################################################
#Functions

function cat_binary {
	
	if [ -e "$FLASHBIN" ]; then
		echo "delete $FLASHBIN"
			rm $FLASHBIN
	fi

	if [ -e "$BOOTLOADERBIN" ]; then
		echo "cat binary begin"
		cp $BOOTLOADERBIN $BOOTLOADERBIN_TMP
		BOOTLOADERBINSIZE=`wc --bytes $BOOTLOADERBIN | cut -d ' ' -f1`
		PADDINGCOUNT=$(($BOOTLOADERMAXSIZE-$BOOTLOADERBINSIZE))
		printf '\377%.0s' $(eval echo "{1..${PADDINGCOUNT}}")  >> $BOOTLOADERBIN_TMP
		echo "padding bootloader binary done"
		
		FREERTOS_BIN_SIZE=`wc --bytes $FREERTOSBIN | cut -d ' ' -f1`
		FREERTOS_PADDING_SIZE=$[$FREERTOS_MAX_SIZE-$FREERTOS_BIN_SIZE]
		cp $FREERTOSBIN $FREERTOSBIN_TMP
		printf '\377%.0s' $(eval echo "{1..${FREERTOS_PADDING_SIZE}}")  >> $FREERTOSBIN_TMP
		echo "padding freertos binary done"
		
		cat $BOOTLOADERBIN_TMP $FREERTOSBIN_TMP $UBIN > $FLASHBIN
		FLASHBINSIZE=`wc --bytes $FLASHBIN | cut -d ' ' -f1`
		echo "$FLASHBIN total size is $FLASHBINSIZE bytes"
		echo "cat binary done"
		rm $BOOTLOADERBIN_TMP $FREERTOSBIN_TMP
	else
		echo "error: no mt2523_bootloader.bin exist"
		exit -1
	fi
}

###############################################################################
cat_binary
exit

###############################################################################
#Perl script start




	

