# Cypress Trusted Firmware-M (TF-M) for PSoC64

## Overview
Trusted Firmware-M is free software that provides secure world software for
Arm Cortex-M processors. It forms the foundations of the Secure Processing
Environment (SPE) of microcontrollers. Cypress has modified TF-M to support
PSoC® 64 devices.

## Licensing
This software component is licensed under a mixture of the Apache License,
version 2 and the 3-Clause BSD License. Please see the individual files to
determine which license applies. Specifically, take note on licensing of the
following modules:
* [t_cose](https://github.com/Infineon/src-trusted-firmware-m/blob/master/lib/ext/t_cose/LICENSE)
* [qcbor](https://github.com/Infineon/src-trusted-firmware-m/blob/master/lib/ext/qcbor/README.md)

## Source files
Source files can be found in [Infineon Trusted Firmware-M git repository](https://github.com/Infineon/src-trusted-firmware-m).

## Quick Start
### Git settings
It is recommended to use following git settings to avoid issues with mismatches
of line-endings:
```bash
# clone as-is, commit as-is
git config --global core.autocrlf false

# forces Git to normalize line endings to LF on checkin and prevents conversion
# to CRLF when the file is checked out
git config --global core.eol lf
```

### Further instructions
For build instructions, ModusToolbox support, provisioning guide and
other PSoC® 64 specific information refer to
[Cypress PSoC® 64 Specifics documentation](https://github.com/Infineon/src-trusted-firmware-m/blob/master/platform/ext/target/cypress/psoc64/cypress_psoc64_spec.rst).

For general TF-M documentation refer to
[TF-M user guide](https://tf-m-user-guide.trustedfirmware.org/index.html) and
[TF-M doc files](https://github.com/Infineon/src-trusted-firmware-m/tree/master/docs).

## More information
The following resources contain more information:
* [Trusted Firmware-M RELEASE.md](https://github.com/Infineon/src-trusted-firmware-m/blob/master/RELEASE.md)
* [Cypress PSoC64 64 Specifics documentation](https://github.com/Infineon/src-trusted-firmware-m/blob/master/platform/ext/target/cypress/psoc64/cypress_psoc64_spec.rst)

*© Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation. All rights reserved.*
