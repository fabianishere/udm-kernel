#!/bin/bash
# Script to built a UDM(P) kernel.

# Print the help message of this script.
usage() {
	cat << EOF

Usage: $0 [OPTIONS...]

Built a custom kernel for the UniFi Dream Machine (Pro).

Options:
  -j            Specify the number of jobs to use for compilation.
  -h, --help    Show this message and exit
EOF
}

jobs=$(nproc)
no_deb=

while getopts ":hj:q" arg; do
case $arg in
    V)
        jobs=$OPTARG
        ;;
    q)
        no_deb=y
        ;;
    h | *) # Display help.
        usage
        exit 0
        ;;
    esac
done
shift $((OPTIND - 1))

# Ensure the cross compilation settings are configured
export ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-

# Export maintainer details
export DEBMAIL="$DEBMAIL" DEBFULLNAME="$DEBFULLNAME"

# Do not append plus to final version
export LOCALVERSION=

# Allow changing extra version
export EXTRAVERSION="$EXTRAVERSION"

echo "Building kernel..."
make -j"$jobs"

if [ -z "$no_deb" ]; then
    echo "Packaging kernel..."
    make -j"$jobs" bindeb-pkg
fi
