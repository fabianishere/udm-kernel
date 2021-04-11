# udm-kernel
Custom Linux kernels for the UniFi Dream Machine (Pro) based on the UbiOS stock kernel. 

See [udm-kernel-tools](https://github.com/fabianishere/udm-kernel-tools)
for instructions on how to run these kernels on your device.

## Features
For the edge flavour, we extend the stock Linux kernel running on the UniFi 
Dream Machine (Pro) with the following features:

1. Multicast routing
2. Multipath routing
3. In-kernel [WireGuard](https://wireguard.com) VPN support

## Installation
SSH into your UniFi Dream Machine and enter the UniFi OS shell as follows:
```bash
unifi-os shell
```

Select from the [Releases](https://github.com/fabianishere/udm-kernel/releases) page the kernel version
you want to install and download the appropriate Debian package. Then,
you can install the package as follows:

```sh
apt install ./udm-kernel-VERSION-RELEASE_arm64.deb
```

Now, to actually use these kernels, please visit [udm-kernel-tools](https://github.com/fabianishere/udm-kernel-tools)
for instructions.

## Building manually
You may also choose to manually build one of these kernels yourself.

#### Prerequisites
Make sure you have the following packages installed:

```bash
apt install build-dep libncurses-dev gawk flex bison openssl libssl-dev gcc-aarch64-linux-gnu
```

It might be that case that more recent versions of GCC have difficulty compiling
the kernel without warnings or issues. The pre-built kernels in this repository
are built on Ubuntu 18.04 using GCC 7.5.0.

#### Obtaining the source
Obtain the kernel sources from Github as follows:
```bash
git clone https://github.com/fabianishere/udm-kernel
cd udm-kernel
```

#### Building
First, prepare the configuration for the kernel. Beforehand, you may add the desired
kernel configuration options to `.github/config/config.local.udm`.
```bash
.github/prepare.sh
```

If you wish to graphically configure the kernel options, pass the `-m` flag:
```bash
.github/prepare.sh -m
```

Once the kernel is configured, build the kernel as follows:
```bash
.github/build.sh
```

Afterwards, you will find the generated Debian packages in the directory above
the repository.

## Questions
If you have any questions or want to have additional features included in the
kernel, feel free to open an issue on Github.
