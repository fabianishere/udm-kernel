# udm-kernel
Custom Linux kernels for the UniFi Dream Machine (Pro) based on the UbiOS stock kernel. 

See [udm-kernel-tools](https://github.com/fabianishere/udm-kernel-tools)
for instructions on how to run these kernels on your device.

## Flavours
This repository offers several kernel flavours for the UniFi Dream Machine (Pro):

### Stock
The sources for the stock kernel running on the UDM/P is
located at the [flavour/stock](https://github.com/fabianishere/udm-kernel/tree/flavour/stock) branch.

We do not offer any pre-built kernels for stock kernels. However, the stock
kernel provides the base upon which the other kernel flavours build.

### Edge
The edge flavour is located at the [master](https://github.com/fabianishere/udm-kernel/tree/master) branch
and extends the stock kernel with more functionality.
In particular, we extend the stock flavour kernel the following features:
1. Multicast routing
2. Multipath routing
3. In-kernel [WireGuard](https://wireguard.com) VPN support

The edge flavour targets the most common use-cases for running a custom kernel,
but should not behave differently from the stock kernel in default configuration.

Pre-built kernels are available on the [Releases](https://github.com/fabianishere/udm-kernel/releases) page
(suffixed with `-edge`).

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

**Important**  
To actually use these kernels, please visit [udm-kernel-tools](https://github.com/fabianishere/udm-kernel-tools)
for instructions.

## Contributing
Feel free to open a issue or pull request to this repository if you have
a question, suggestion or want to add new functionality to the custom kernels.

If you are adding new functionality to the kernel, please motivate your use-case
and think about how it might be useful for other users as well.

## Build Process

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