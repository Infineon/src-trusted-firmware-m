# Cypress Trusted Firmware-M (TF-M) for PSoC64

## Overview

Trusted Firmware-M is free software that provides secure world software for Arm Cortex-M processors. It forms the foundations
of the Secure Processing Environment (SPE) of microcontrollers. To work with Cypress devices, Cypress has modified TF-M to support PSoC 64 devices.

This software component is licensed under a mixture of the Apache License, version 2 and the 3-Clause BSD License. Please see the individual files to determine which license applies. Source files can be found in Trusted Firmware-M [GIT](https://git.trustedfirmware.org/trusted-firmware-m.git). Specifically, take note of the following modules.
* [t_cose](https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/tree/lib/ext/t_cose/LICENSE)
* [qcbor](https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/tree/lib/ext/qcbor/README.md)


## Requirements

* [ModusToolbox® software](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software) v2.4
* Board Support Package (BSP) minimum required version: 3.0.0
* PDL version: 2.4.0
* Programming Language: C
* Associated Parts: See "Supported Kits" section below.

## Supported Toolchains

* GNU Arm® Embedded Compiler v9.3.1
* Arm compiler v6.13
* IAR C/C++ compiler v8.42.1

## Supported Kits

* [PSoC® 64 Standard Secure - AWS Wi-Fi BT Pioneer Kit (CY8CKIT-064S0S2-4343W)](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-064s0s2-4343w)

## Features

The current implementation supports the following services:
* PSA Crypto
* PSA Initial Attestation
* PSA Internal Trusted Storage
* PSA Protected Storage
These services are defined by the [PSA API](https://github.com/ARM-software/psa-arch-tests/tree/master/api-specs)

## Quick Start

### ModusToolBox support
Refer to the Building Multi-Core TF-M with ModusToolBox section in [Cypress PSoC64 Specifics documentation](https://github.com/Infineon/src-trusted-firmware-m/blob/master/platform/ext/target/cypress/psoc64/cypress_psoc64_spec.rst)


### Provisioning the kit
Refer to the [Provisioning Guide](https://www.cypress.com/file/521106/download)


## More information
The following resources contain more information:
* [Trusted Firmware-M RELEASE.md](./RELEASE.md)
* [PSA API](https://github.com/ARM-software/psa-arch-tests/tree/master/api-specs)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software)
* [Cypress Semiconductor Corporation (an Infineon company)](https://www.infineon.com)

Please use following settings for git if you have issues with patches which are caused by mismatches of line-endings:

```
# clone as-is, commit as-is
git config --global core.autocrlf false

# forces Git to normalize line endings to LF on checkin and prevents conversion to CRLF when the file is checked out
git config --global core.eol lf
```

The linker files included with TF-M must be generic and handle all common use cases. Your project may not use every
section defined in the linker files. In that case you may see the warnings during the build process using ARM clang toolchain :
``L6329W (pattern only matches removed unused sections).``
In your project, you can suppress the warning by passing the ``-DTFM_LINK_OPTIONS=--diag_suppress=6329`` option to the linker,
simply comment out or remove the relevant code in the linker file.

*© Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation. All rights reserved.*
