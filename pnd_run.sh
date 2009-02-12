#!/bin/bash
 
#input pnd_run.sh -p "/path/to/foobar.pnd" -e "exe" --a "arguments for exe"
#output none
 
########################### FS NAMES NEED ADJUSTMENT #################
#todo
# check if all vars are set to sensible values
 
# parse arguments
TEMP=`getopt -o p:e:a:: --long p-long,e-long:,a-long: -- "$@"`
 
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi
 
# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"
 
while true ; do
	case "$1" in
		-p|--p-long) echo "pnd set to \`$2'" ;PND=$2;shift 2;;
		-e|--e-long) echo "exec set to \`$2'" ;EXENAME=$2;shift 2 ;;
		-a|--a-long) 
			# a has an optional argument. As we are in quoted mode,
			# an empty parameter will be generated if its optional
			# argument is not found.
			case "$2" in
				"") echo "no arguments"; shift 2 ;;
				*)  echo "args set to \`$2'" ;ARGUMENTS=$2;shift 2 ;;
			esac ;;
		--) shift ; break ;;
		*) echo "Error while parsing arguments!" ; exit 1 ;;
	esac
done
 
# add sanity check
 
#vars
DFS=$(file -b $PND | awk '{ print $1 }') # is -p a zip iso or folder?
MOUNTPOINT=$(df $PND | grep -vE '^Filesystem' | awk '{ print $6  }') #find out which mountpoint the pnd/folder is on
BASENAME=$(basename "$PND" | cut -d'.' -f1) #get basename (strip extension if file) for union mountpoints etc, maybe  this should be changed to something specified inside the xml
oCWD=$(pwd)
# add sanity check
 
#detect fs
if [ $DFS = ISO ]; then
	mntline="mount -o loop $PND /mnt/pnd/$BASENAME"
	echo "Filetype is $DFS"
elif [ $DFS = Zip ]; then
	mntline="fuse-zip $PND /mnt/pnd/$BASENAME -oro" #should be reight now
	echo "Filetype is $DFS"
elif [ $DFS = directory ]; then
	mntline="mount --bind -o ro $PND /mnt/pnd/$BASENAME"
#we bind the folder, now it can be treated in a unified way ATENTION: -o ro doesnt work for --bind at least on 25, on 26 its possible using remount, may have changed on 27
	echo "Filetype is $DFS"
else
	echo "error"
	exit 1;
fi
 
#create mountpoints
#echo "
#will run:
# create mountpoints
mkdir -p /mnt/pnd/$BASENAME
mkdir -p $MOUNTPOINT/appdata/$BASENAME
mkdir -p /mnt/utmp/$BASENAME
 
#mount
$mntline #mount the pnd/folder
#mount -t unionfs -o exec,dirs\=$MOUNTPOINT/appdata/$BASENAME=rw:/mnt/pnd/$BASENAME=ro unionfs /mnt/utmp/$BASENAME #union mount
#aufs, one of those should work, bit unsure.
mount -t aufs -o exec,dirs\=$MOUNTPOINT/appdata/$BASENAME=rw:/mnt/pnd/$BASENAME=ro none /mnt/utmp/$BASENAME #aufs?
#mount -t aufs -o exec,dirs\=$MOUNTPOINT/appdata/$BASENAME=rw:/mnt/pnd/$BASENAME=ro aufs /mnt/utmp/$BASENAME #aufs?
 
 
#start app
cd /mnt/utmp/$BASENAME
$EXENAME $ARGUMENTS 
cd $oCWD
#app exited
 
#clean up
umount /mnt/utmp/$BASENAME
umount /mnt/pnd/$BASENAME
rmdir /mnt/pnd/$BASENAME
rmdir /mnt/utmp/$BASENAME
#"

