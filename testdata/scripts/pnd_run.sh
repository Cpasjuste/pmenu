#!/bin/bash
 
#Usage: pnd_run.sh -p your.pnd -e executeable [-a \"(arguments)\"] [ -s \"cd to folder inside pnd\"] [-u (skip union)] [-b override BASENAME (name of mountpoint/appdata)]
# -u to skip union mount, should probably be removed before release
# -s startdir
# arguments can be inside -e, -a is optional
 
#/etc/sudoers needs to be adjusted if you touch any of the sudo lines in the wrong place.
 
 
# parse arguments
TEMP=`getopt -o p:e:a:b:s:u:: -- "$@"`
 
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi
 
# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"
 
while true ; do
	case "$1" in
		-p) echo "pnd set to \`$2'" ;PND=$2;shift 2;;
		-e) echo "exec set to \`$2'" ;EXENAME=$2;shift 2 ;;
		-u) echo "u set, no union pls!";NOUNION=1;shift 2 ;;
		-b) echo "BASENAME set to $2";BASENAME=$2;shift 2;;
		-s) echo "startdir set to $2";STARTDIR=$2;shift 2;;
		-a) 
			case "$2" in
				"") echo "no arguments"; shift 2 ;;
				*)  echo "args set to \`$2'" ;ARGUMENTS=$2;shift 2 ;;
			esac ;;
		--) shift ; break ;;
		*) echo "Error while parsing arguments!" ; exit 1 ;;
	esac
done
 
if [ ! $PND ] || [ ! $EXENAME ]; then
	echo "Usage: pnd_run.sh -p your.pnd -e executeable [-a \"(arguments)\"] [ -s \"cd to folder inside pnd\"] [-u (skip union)] [-b override BASENAME (name of mountpoint/appdata)]"
	exit 1
fi
 
#vars
DFS=$(file -b $PND | awk '{ print $1 }') #is -p a zip/iso or folder?
MOUNTPOINT=$(df $PND | grep -vE '^Filesystem' | awk '{ print $6  }') #find out which mountpoint the pnd/folder is on, there probably is a better way to do this
 
 
#BASENAME really should be something sensible and somewhat unique
#if -b is set use that as basename, else generate it from PND
#get basename (strip extension if file) for union mountpoints etc, maybe  this should be changed to something specified inside the xml
#this should probably be changed to .... something more sensible
if [ ! $BASENAME ]; then BASENAME=$(basename "$PND" | cut -d'.' -f1) ; fi
 
 
 
oCWD=$(pwd)
 
#detect fs
if [ $DFS = ISO ]; then
	mntline="mount -o loop,mode=777 $PND /mnt/pnd/$BASENAME"
	echo "Filetype is $DFS"
elif [ $DFS = Zip ]; then
	mntline="fuse-zip $PND /mnt/pnd/$BASENAME -o ro,fmask=000" #TOTALLY untested right now
	echo "Filetype is $DFS"
elif [ $DFS = directory ]; then
	mntline="mount --bind -o ro $PND /mnt/pnd/$BASENAME"
#we bind the folder, now it can be treated in a unified way ATENTION: -o ro doesnt work for --bind at least on 25, on 26 its possible using remount, may have changed on 27
	echo "Filetype is $DFS"
else
	echo "error"
	exit 1;
fi
 
#create mountpoints, check if they exist already first to avoid annoying error messages
if [ ! -d /mnt/pnd/$BASENAME ]; then mkdir -p /mnt/pnd/$BASENAME ; fi
if [ ! -d $MOUNTPOINT/appdata/$BASENAME ]; then mkdir -p $MOUNTPOINT/appdata/$BASENAME; fi
if [ ! -d /mnt/utmp/$BASENAME ]; then mkdir -p /mnt/utmp/$BASENAME; fi 
 
#mount
if [ ! $NOUNION ] ; then
	#is the union already mounted? if not mount evrything, else launch the stuff
	mount | grep "on /mnt/utmp/$BASENAME type" # > /dev/null
	if [ ! $? -eq 0 ]; then 
		echo "mounting union!"
		$mntline #mount the pnd/folder
		mount -t aufs -o exec,dirs\=$MOUNTPOINT/appdata/$BASENAME=rw+nolwh:/mnt/pnd/$BASENAME=rr none /mnt/utmp/$BASENAME # put union on top
 
	else
		echo "Union already mounted"
	fi
 
 
	#start app
	cd /mnt/utmp/$BASENAME
	if [ $STARTDIR ]; then cd $STARTDIR; fi #cd to folder specified by the optional arg -s
	./$EXENAME $ARGUMENTS
	#app exited
	cd $oCWD #cd out of the mountpoint so we can umount, doesnt really matter to where...
 
else
 
	$mntline
	cd /mnt/pnd/$BASENAME
	if [ $STARTDIR ]; then cd $STARTDIR; fi
	echo $(pwd)
	./$EXENAME $ARGUMENTS 
	cd $oCWD
fi
 
#clean up
 
if [ ! $NOUNION ] ; then umount /mnt/utmp/$BASENAME; fi #umount union if -u wasnt set
if [ $NOUNION ] ; then umount /mnt/pnd/$BASENAME; fi #umount iso if -u WAS set
if [ $? -eq 0 ]; then # check if the umount was successfull, if it wasnt it would mean that theres still something running so we skip this stuff
	if [ ! $NOUNION ] ; then
		umount /mnt/pnd/$BASENAME
		rmdir $MOUNTPOINT/appdata/$BASENAME/.wh..wh.plink 
		rmdir $MOUNTPOINT/appdata/$BASENAME/
		rmdir /mnt/utmp/$BASENAME;
	fi
	rmdir /mnt/pnd/$BASENAME 
fi
