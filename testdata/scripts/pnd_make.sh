#!/bin/bash
 
######adjust path of genpxml.sh if you want to use that "feture"#####
 
TEMP=`getopt -o p:d:x:i: -- "$@"`
 
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi
 
eval set -- "$TEMP"
while true ; do
	case "$1" in
		-p) echo "PNDNAME set to $2" ;PNDNAME=$2;shift 2;;
		-d) echo "FOLDER set to $2" ;FOLDER=$2;shift 2 ;;
		-x) echo "PXML set to $2" ;PXML=$2;shift 2 ;;
		-i) echo "ICON set to $2" ;ICON=$2;shift 2 ;;
		--) shift ; break ;;
		*) echo "Error while parsing arguments! $2" ; exit 1 ;;
	esac
done
 
rnd=$RANDOM; # random number for genpxml and index$rnd.xml
 
#generate pxml if guess or empty
if [ ! $PXML ] || [ $PXML = "guess" ] && [  $PNDNAME ] && [ $FOLDER ];  then
	PXMLtxt=$(./genpxml.sh $FOLDER $ICON)
	PXML=tmp$rnd.pxml
	echo "$PXMLtxt" > tmp$rnd.pxml
fi
 
#check arguments
if [ ! $PNDNAME ] || [ ! $FOLDER ] || [ ! $PXML ]; then
	echo " Usage: pnd_make.sh -p your.pnd -d folder/containing/your/app/ -x 
	your.pxml (or \"guess\" to try to generate it from the folder) -i icon.png"
	exit 1
fi
if [ ! -d $FOLDER ]; then echo "$FOLDER doesnt exist"; exit 1; fi #check if folder actually exists
if [ ! -f $PXML ]; then echo "$PXML doesnt exist"; exit 1; fi #check if pxml actually exists
 
#make iso from folder
mkisofs -o $PNDNAME.iso -R $FOLDER
#append pxml to iso
cat $PNDNAME.iso $PXML >  $PNDNAME
rm $PNDNAME.iso #cleanup
 
#append icon if specified
if [ $ICON ]; then # check if we want to add an icon
	if [ ! -f $ICON ]; then #does the icon actually exist?
		echo "$ICON doesnt exist"
	else # yes
	mv $PNDNAME $PNDNAME.tmp
	cat $PNDNAME.tmp $ICON > $PNDNAME # append icon
	rm $PNDNAME.tmp #cleanup
	fi
fi
 
if [ $PXML = "guess" ];then rm tmp$rnd.pxml; fi #cleanup
