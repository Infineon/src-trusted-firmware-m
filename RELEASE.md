# Cypress Customized Trusted Firmware-M Release Notes

## What's Included?
1. TF-M implementation with support of the following services:
    * Crypto
    * Initial Attestation
    * Internal Trusted Storage
    * Platform
    * Protected Storage
2. Support for the following dual core PSoC® 64 devices:
    * CYS0644ABZI-S2D44 MCU with implementation for the following reference
      boards:
        * cy8ckit-064s0s2-4343w
    * CYB0644ABZI-S2D44 MCU with implementation for the following reference
      boards:
        * cy8ckit-064b0s2-4343w
3. PSoC® 64 reference policies and sample image signing keys
4. Helper script to facilitate device re-provisioning
5. Detailed PSoC® 64 specific documentation
6. Support of TF-M in ModusToolbox in both precompiled binary and source
   code forms

See the [README.md](https://github.com/Infineon/src-trusted-firmware-m/blob/master/README.md)
for an additional description of the TF-M software.

## Changelog

### v1.3.110
* Added GCC11 support
* Fixed build of p64_utils for non-secure image with cmake (non ModusToolbox)
* Fixed build of TRNG driver for PDL v3.2.0 and upward
* Fix TFM_LINK_SET_RW_IN_PARTITION_SECTION for IAR

### v1.3.100
* Changed TF-M ModusToolbox library structure to support ModusToolbox 3.0
  (this library is incompatible with ModusToolbox 2.4 or earlier)
* Policy files updated for CySecureTools 4.1.0
  (Policy files are incompatible with CySecureTools 4.0.0 or earlier)
* Added support for CySecureTools 4.1.0 to `reprov_helper.py` script
* Updated PSoC® 64 specific documentation
* Updated to use Peripheral Driver Library v3.0.0
* Added external CMSIS-Core library v5.5.0 for PDL
* When CY_POLICY_CONCEPT=ON, boards provisioned with a policy where uart_base
  is not 1080360960 will fail to boot rather than just disabling the UART
* Fixed documentation build

### v1.3.5
* Updated to use PSoC64 Secure Boot Utilities Middleware Library v1.0.1
* Fixed known issue related to the constant value (100ms) for acquire debug
  window. If `CY_HW_SETTINGS_FROM_POLICY` is defined, the CM4 debug permissions
  specified in the policy used to provision the board will be respected and the
  `"acq_win"` from the policy will be used to determine how long to wait for a
  debugger connection on the CM4 AP. If `CY_HW_SETTINGS_FROM_POLICY` is not
  defined, this behaviour is controlled by the `hw_settings` structure.
* Updated `cm4-app-example.mk` template for ModusToolBox to simplify switching
  from sources to binary.

### v1.3.3
* Updated to use PDL v2.4.0
* Updated TFM to use core-lib v1.3.0 as dependency
* Added `COMPONENT_TFM_S_SRC` component to the trusted-firmware-m ModusToolbox
  library to support building of secure image from sources.
* Added possibility to remove policy parsing concept with `CY_POLICY_CONCEPT=OFF`.
  Refer to *Optional arguments* section in documentation
  [Cypress PSoC64 Specifics](https://github.com/Infineon/src-trusted-firmware-m/blob/master/platform/ext/target/cypress/psoc64/cypress_psoc64_spec.rst)
* Updated ECC Crypto algorithms to use software implementation only, due to
  security concerns for HW acceleration. See details in
  [cypress_mxcrypto_ecc_vulnerability.rst](./docs/reference/security_advisories/cypress_mxcrypto_ecc_vulnerability.rst)
* Increased performance of PS/ITS by setting `ITS_BUF_SIZE` and `ITS_MAX_ASSET_SIZE`
  to default values in Small and Medium profiles.
* Provided possibility to have out-of-tree custom partitions.
* Added `TFM_LINK_OPTIONS` to provide additional options for linker.

### v1.3.1
* Update to PDL v2.3.0

### v1.3.0
* Update to TFM v1.3.0 from trustedfirmware.org
* Update to PDL v2.2.0
* Update to use p64_utils v1.0.0

### v1.2.0
* Update to TFM v1.2.0 from trustedfirmware.org
* Update to PDL v1.6.1

### v1.0.0
* Initial release for TF-M

## Known Issues
* Currently, IPC semaphores are not safe from security point of view and are
  not recommended to use
* Due to changes in TF-M 1.3.3 the existing latency could be not enough, please
  take care about latency update, if any issues observed
  (for example `CY_CFG_PWR_DEEPSLEEP_LATENCY` in FreeRTOS).

## Supported Platforms
This library and its features are supported on the following Cypress platforms:
* [PSoC® 64 Standard Secure - AWS Wi-Fi BT Pioneer Kit (CY8CKIT-064S0S2-4343W)](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-064s0s2-4343w)
* [PSoC® 64 "Secure Boot" Wi-Fi BT Pioneer Kit (CY8CKIT-064B0S2-4343W)](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-064b0s2-4343w)

## Supported Software and Tools
This version of TF-M was validated for compatibility with the following Software and Tools:

| Software and Tools                                                            | Version       |
| :---                                                                          | :----         |
| ModusToolbox Software Environment                                             | 2.4.0         |
| GCC Compiler                                                                  | 9.3.1         |
| ARM Compiler 6                                                                | 6.13          |
| IAR C/C++ compiler                                                            | 8.42.1        |
| CMake                                                                         | 3.15          |
| CMSIS-Core(M)                                                                 | 5.5.0         |
| MbedTLS                                                                       | 2.25.0        |
| PSoC6 MCUs acceleration for mbedTLS library                                   | 1.3.0         |
| PSoC64 Secure Boot Utilities Middleware Library                               | 1.0.1         |
| PSoC 6 Peripheral Driver Library (PDL)                                        | 3.0.0         |
| CY8CKIT-064S0S2-4343W Board Support Package (BSP)                             | 3.0.0         |
| CY8CKIT-064B0S2-4343W Board Support Package (BSP)                             | 3.0.0         |
| Core Library                                                                  | 1.3.0         |
| RTOS Abstraction                                                              | 1.4.0         |
| CySecureTools                                                                 | 4.1.0         |

Minimum required ModusToolbox Software Environment: v2.4.0

## More information
Use the following links for more information, as needed:
* [Cypress Semiconductor Corporation (an Infineon company)](https://www.infineon.com)
* [Cypress Semiconductor Corporation (an Infineon company) GitHub](https://github.com/Infineon)
* [PSoC® 64 Secure Microcontrollers](https://www.infineon.com/cms/en/product/microcontroller/32-bit-psoc-arm-cortex-microcontroller/psoc-6-32-bit-arm-cortex-m4-mcu/psoc-64)
* [Trusted Firmware website](https://www.trustedfirmware.org)
* [Cypress PSoC64 Specifics documentation](https://github.com/Infineon/src-trusted-firmware-m/blob/master/platform/ext/target/cypress/psoc64/cypress_psoc64_spec.rst)
* [TF-M project](https://www.trustedfirmware.org/projects/tf-m)
* [PSA API](https://github.com/ARM-software/psa-arch-tests/tree/master/api-specs)
* [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software)

---
*© Copyright (c) 2020-2022 Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation. All rights reserved.*
