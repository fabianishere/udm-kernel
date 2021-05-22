#!/bin/bash

# Converts known types of register dumps to simple format - <register_addr>: <value>
# Usage: reg-dump-conv.sh <reg dump file path> <format: app/al_tool/rh_tool/devmem2/uboot> [output file path]
# ---------------------------------------------------------------------------------------------------

set -e

OUTPUT_DIR=`mktemp -d`
mkdir -p $OUTPUT_DIR/

# Format example: "nb_service->global.cpus_config [0xf0070000] = 0x0000001f"
convert_app () {
	local val=''
	local reg=''

	while read LINE
	do
		if [[ $LINE =~ \[$addrpat\] ]]
		then
			reg=$( echo $LINE | sed -s 's/.*\[0x\(.*\)\].*/\1/')
			val=$( echo $LINE | sed -s 's/.*\= 0x\(.*\).*/\1/')
			echo $reg': '$val >> $out
		fi
	done
}

#Format example: "Value of register 0x000001E0: 0xFB000000
#				  eth0_adapter->gen_ctrl_reg_1.data (0xfc0001e4):
convert_al_tool () {
	local val=''
	local reg=''

	while read LINE
	do
		if [[ $LINE =~ \($addrpat\) ]]
		then
			reg=$( echo $LINE | sed -s 's/.*(0x\(.*\)).*/\1/')
			read LINE
			val=$( echo $LINE | sed -s 's/.*\: 0x\(.*\).*/\1/')
			echo $reg': '$val >> $out
		fi
	done
}



#Format example: "[0xf0070000] = ffffffff | nb_service->global.sec_ovrd_low"
convert_rh_tool () {
	local val=''
	local reg=''

	while read LINE
	do
		if [[ $LINE =~ \[$addrpat\] ]]
		then
			reg=$( echo "$LINE" | sed -s 's/\[0x\(.*\)\].=.\(........\) .*/\1/')
			val=$( echo "$LINE" | sed -s 's/\[0x\(.*\)\].=.\(........\) .*/\2/')
			echo $reg': '$val >> $out
		fi
	done
}

# Format example: "nb_service->global.cpus_config:
#				   Value at address 0xF0070000 (0x7fa3741000): 0x1F"
convert_devmem2 () {
	local val=''
	local reg=''

	while read LINE
	do
		if [[ $LINE =~ $addrpat ]]
		then
			reg=$( echo $LINE | sed -s 's/.*\ 0x\(.* \).*/\1/' | \
					sed -s 's/(\(.* \).*//' )
			val=$( echo $LINE | sed -s 's/.*\0x\(.*\).*/\1/' )
			echo $reg': '$val >> $out
		fi
	done
}

# Format example: "nb_service->global.cpus_config
#				   f0070000: 0000001f                               ...."
convert_uboot () {
	# Format is already fine, simply copy to output
	cp $in $out
}

activate_conversion () {
	case $1 in
		app)
			convert_app
			;;
		al_tool)
			convert_al_tool
			;;
		rh_tool)
			convert_rh_tool
			;;
		devmem2)
			convert_devmem2
			;;
		uboot)
			convert_uboot
			;;
	esac
}

# First check if at least one parameter given
if [ $# -lt 2 ]
then
	echo "Usage: $0 <dump-file> <format> [output-file]"
	exit 1
fi

formats_list="app al_tool rh_tool devmem2 uboot"

if [[ $formats_list =~ (^|[[:space:]])$2($|[[:space:]]) ]]
then
	format_type=$2
else
	echo "Format should be one of: "
	echo $formats_list
	exit 1
fi

addrpat='0xf[0-9a-f]{7}'

# remember the name of the input file
in=$1

# init
canonical="$OUTPUT_DIR/canonical_dump.txt"
out=/dev/stdout

if [ $# -gt 2 ]
then
	if [ -f $3 ]
	then
		rm -f $3
	fi
	out=$3
fi

# Lowercase contents of file
echo "$(tr '[:upper:]' '[:lower:]' <  $in)" >> $canonical

# Link filedescriptor 10 with stdin
exec 10<&0

# stdin replaced with the canonical file
exec < $canonical

activate_conversion $format_type

#############################################
################## clean up #################
#############################################

rm -rf $OUTPUT_DIR/

# restore stdin from filedescriptor 10
# and close filedescriptor 10
exec 0<&10 10<&-
