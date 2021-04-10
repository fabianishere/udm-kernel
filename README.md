# udm-kernel
Custom Linux kernels for the UniFi Dream Machine (Pro). See [udm-kernel-tools](https://github.com/fabianishere/udm-kernel-tools)
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

## Questions
If you have any questions or want to have additional features included in the
kernel, feel free to open an issue on Github.
