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

while getopts ":hj:" arg; do
case $arg in
    V)
        jobs=$OPTARG
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

echo "Building kernel..."
make -j"$jobs"

echo "Packaging kernel..."
make deb-pkg
