########################
Cypress PSoC64 Specifics
########################

.. contents:: Table of Contents

*****************
Supported devices
*****************

For the list of supported devices refer to ``RELEASE.md`` document.

*************
Prerequisites
*************

Software requirements
=====================

General tools
-------------

Install required software as described in
:doc:`Software requirements TF-M document </docs/getting_started/tfm_sw_requirement>`.

CySecureTools and PyOCD
-----------------------

Install CySecureTools or update it to the newest version.

.. code-block:: bash

    pip3 install --upgrade --force-reinstall cysecuretools

PyOCD is installed by CySecureTools as a dependency.

For more details please refer to
`CySecureTools page <https://pypi.org/project/cysecuretools>`_.

OpenOCD
-------

Install OpenOCD with PSoC6 support. Download the latest version (version 4.1.0
or newer) from
`Infineon OpenOCD github repository <https://github.com/Infineon/openocd/releases>`_.

Firmware loader
---------------

Download the latest version of fw-loader from
`Infineon Firmware-loader github repository <https://github.com/Infineon/Firmware-loader/releases>`_.

Update board firmware
=====================

PSoC64 board KitProg firmware needs to be 2.00.744 version or greater.
Please use Modus Toolbox or CyProgrammer to update it if needed. Refer to
`KitProg user guide <https://www.infineon.com/dgdl/Infineon-KitProg3_User_Guide-UserManual-v01_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f01221f1853>`_
for more details.

Provision the board
===================

PSoC64 must first be provisioned with SecureBoot firmware and a provisioning
packet containing policy and secure keys. Refer to the Secure Boot SDK User
Guide [1]_ and PSoC64 provisioning specification [2]_ for more information
on PSoC device provisioning.

Generating the keys
-------------------

There are several keys that are referenced in TF-M specific policies:

    * TFM_S_KEY.json      - private OEM key for signing CM0+ image
    * TFM_S_KEY_PRIV.pem  - private OEM key for signing CM0+ image in PEM format
    * TFM_NS_KEY.json     - private OEM key for signing CM4 image
    * TFM_NS_KEY_PRIV.pem - private OEM key for signing CM4 image in PEM format

See `Provisioning policies`_ section for more details on TF-M specific policies.

If re-provisioning is being done using `reprov_helper.py` script, new keys can
be generated as part of the provisioning process. If you want to generate keys
as a separate step, follow this process. Use cysecuretools to generate new key
pairs defined by the policy file, for example:

.. code-block:: bash

    # Replace <name_of_the_board> with name of the used board (e.g. CY8CKIT-064S0S2-4343W)
    BOARD_NAME=<name_of_the_board>
    cd platform/ext/target/cypress/psoc64/security/COMPONENT_${BOARD_NAME}
    cysecuretools -t ${BOARD_NAME} -p policy/policy_multi_CM0_CM4_tfm.json create-keys
    # Be sure to backup keys to a safe place

Signing keys have to be provisioned to the board, refer to
`Performing provisioning`_ section for more details on device provisioning.

Performing provisioning
-----------------------

Depending on the policy file, board can be provisioned with or without device
certificates and/or Amazon Web Services root certificates. To choose the policy
that suits the needs of your project refer to the `Provisioning policies`_
section.

To provision the device:

    1. Choose the needed policy file.
    2. Optionally generate signing keys or use existing keys. Refer to
       `Generating the keys`_ section for more details on key generation.
    3. If not done yet, initialize cysecuretools environment in psoc64 board
       security directory:

       .. code-block:: bash

            # Replace <name_of_the_board> with name of the used board (e.g. CY8CKIT-064S0S2-4343W)
            BOARD_NAME=<name_of_the_board>
            cd platform/ext/target/cypress/psoc64/security/COMPONENT_${BOARD_NAME}
            cysecuretools -t ${BOARD_NAME} init

    4. If used policy references certificates then create new (or copy existing)
       certificates to ``certificates`` directory next to the
       ``policy`` directory.
    5. Switch the board to DAPLink mode (refer to `Switching to DAPLink mode`_
       for more details).
    6. Run ``reprov_helper.py`` script. If running the script with default
       parameters, the script can be run as following:

       .. code-block:: bash

            python reprov_helper.py

       To get the full list of options, run the script with ``--help``
       parameter.
    7. Confirm selected options. When prompted for a serial number, enter the
       board's unique serial number (digits only, e.g. 00183).
    8. The script will ask if you want to create new signing keys.
       Answer:

        * ``Yes`` to generate new signing keys in the keys directory.

        .. danger::

            Choosing ``Yes`` option will overwrite existing keys.

        * ``No`` to retain and use the existing keys.

       After re-provisioning, from now on any images for this board will have to
       be signed with these keys.

       .. warning::

        Be sure to backup keys to a safe place.

    9. The script will erase user images.
       If used policy references device certificates, the script will read
       device public key and create device certificates based on the board
       serial number, root certificate and the device public key.

*************************************************************
Using Multi-Core TF-M for Cypress PSoC64 without ModusToolBox
*************************************************************

.. note::

    Commands in this section are provided for reference boards. If custom
    board is used, commands need to be adjusted to account for specifics of
    this board.

Configuring the build
=====================

The build configuration for TF-M is provided to the build system using command
line arguments. Required arguments are noted below:

   * ``-DTFM_PLATFORM=cypress/psoc64`` - target platform name: .
   * ``-DTFM_TOOLCHAIN_FILE=<path to toolchain file>`` - compiler toolchain
     file. There are several toolchains supported:

        * ``<TF-M root dir>/toolchain_ARMCLANG.cmake``
        * ``<TF-M root dir>/toolchain_GNUARM.cmake``
        * ``<TF-M root dir>/toolchain_IARARM.cmake``

For more details on CMake configuration arguments refer to the
`Optional CMake configuration arguments`_ section.

Build Instructions
==================

For generic build instructions refer to
:doc:`TF-M build instructions document </docs/getting_started/tfm_build_instruction>`

To build TF-M for PSoC64 first invoke CMake to configure the build, then invoke
GNU make to compile the project.

.. note::

    There are two ways to invoke GNU make command on build directory, the
    results of both ways are identical, you can use preferred one:

    * using GNU make command:

      .. code-block:: bash

        cd <build folder>
        make <GNU make options>

    * using CMake command. Under the hood this will invoke GNU make command.

      .. code-block:: bash

        cmake --build <build folder> -- <GNU make options>

Here are examples of several build configurations (note that both the compiler
and the debugging type can be changed to other configurations):

    * Build multi-core TF-M at Isolation Level 1 without regression test suites:

      .. code-block:: bash

        cd <trusted-firmware-m folder>
        # Replace <folder_name> with the desired name of the build folder
        BUILD_FOLDER=<folder_name>

        cmake -S . -B ${BUILD_FOLDER} -G "Unix Makefiles" \
              -DTFM_PLATFORM=cypress/psoc64 \
              -DTFM_TOOLCHAIN_FILE=./toolchain_GNUARM.cmake

        cmake --build ${BUILD_FOLDER} -- install

    * Build multi-core TF-M at Isolation Level 1 with regression test suites:

      .. code-block:: bash

        cd <trusted-firmware-m folder>
        # Replace <folder_name> with the desired name of the build folder
        BUILD_FOLDER=<folder_name>

        cmake -S . -B ${BUILD_FOLDER} -G "Unix Makefiles" \
              -DTFM_PLATFORM=cypress/psoc64 \
              -DTFM_TOOLCHAIN_FILE=./toolchain_GNUARM.cmake \
              -DTEST_S=ON -DTEST_NS=ON

        cmake --build ${BUILD_FOLDER} -- install

    * Build multi-core TF-M at Isolation Level 1 with PSA API test suite for the
      attestation service:

      .. code-block:: bash

        cd <trusted-firmware-m folder>
        # Replace <folder_name> with the desired name of the build folder
        BUILD_FOLDER=<folder_name>

        cmake -S . -B ${BUILD_FOLDER} -G "Unix Makefiles" \
              -DTFM_PLATFORM=cypress/psoc64 \
              -DTFM_TOOLCHAIN_FILE=./toolchain_GNUARM.cmake \
              -DTEST_PSA_API=INITIAL_ATTESTATION

        cmake --build ${BUILD_FOLDER} -- install

    * Build multi-core TF-M at Isolation Level 2 without regression test suites:

      .. code-block:: bash

        cd <trusted-firmware-m folder>
        # Replace <folder_name> with the desired name of the build folder
        BUILD_FOLDER=<folder_name>

        cmake -S . -B ${BUILD_FOLDER} -G "Unix Makefiles" \
              -DTFM_PLATFORM=cypress/psoc64 \
              -DTFM_TOOLCHAIN_FILE=./toolchain_GNUARM.cmake \
              -DTFM_ISOLATION_LEVEL=2

        cmake --build ${BUILD_FOLDER} -- install

    * Build multi-core TF-M at Isolation Level 2 with regression test suites:

      .. code-block:: bash

        cd <trusted-firmware-m folder>
        # Replace <folder_name> with the desired name of the build folder
        BUILD_FOLDER=<folder_name>

        cmake -S . -B ${BUILD_FOLDER} -G "Unix Makefiles" \
              -DTFM_PLATFORM=cypress/psoc64 \
              -DTFM_TOOLCHAIN_FILE=./toolchain_GNUARM.cmake \
              -DTFM_ISOLATION_LEVEL=2 \
              -DTEST_S=ON -DTEST_NS=ON

        cmake --build ${BUILD_FOLDER} -- install

    * Build multi-core TF-M at Isolation Level 2 with PSA API test suite for the
      protected storage:

      .. code-block:: bash

        cd <trusted-firmware-m folder>
        # Replace <folder_name> with the desired name of the build folder
        BUILD_FOLDER=<folder_name>

        cmake -S . -B ${BUILD_FOLDER} -G "Unix Makefiles" \
              -DTFM_PLATFORM=cypress/psoc64 \
              -DTFM_TOOLCHAIN_FILE=./toolchain_GNUARM.cmake \
              -DTFM_ISOLATION_LEVEL=2 \
              -DTEST_PSA_API=PROTECTED_STORAGE

        cmake --build ${BUILD_FOLDER} -- install

You can use:

    * ``-j`` GNU make option for multithreaded build
    * ``VERBOSE=1`` GNU make option for verbose build

Signing the images
==================

Sign the images using CySecureTools CLI tool.

.. note::

    CySecureTools overwrites the unsigned file with a signed one, it also
    creates an unsigned copy <filename>_unsigned.hex.

The following code can be used to sign the images:

.. code-block:: bash

    # Replace <name_of_the_board> with name of the used board (e.g. CY8CKIT-064S0S2-4343W)
    BOARD_NAME=<name_of_the_board>
    # Specify the name of the policy used to provision the device
    POLICY_NAME=policy_multi_CM0_CM4_tfm.json
    # Replace <folder_name> with the desired name of the build folder
    BUILD_FOLDER=<folder_name>

    # Sign TF-M secure image
    cysecuretools \
    --policy platform/ext/target/cypress/psoc64/security/COMPONENT_${BOARD_NAME}/policy/${POLICY_NAME} \
    --target ${BOARD_NAME} \
    sign-image \
    --hex ${BUILD_FOLDER}/bin/tfm_s.hex \
    --image-type BOOT \
    --image-id 1

    # Sign non-secure image
    cysecuretools \
    --policy platform/ext/target/cypress/psoc64/security/COMPONENT_${BOARD_NAME}/policy/${POLICY_NAME} \
    --target ${BOARD_NAME} \
    sign-image \
    --hex ${BUILD_FOLDER}/bin/tfm_ns.hex \
    --image-type BOOT \
    --image-id 16

Signing options:

    * ``--image-type`` option:

        * ``--image-type BOOT`` - creates a signed hex file with offsets
          for the primary image slot.
        * ``--image-type UPGRADE`` - creates a signed hex file with offsets
          for the secondary (upgrade) image slot. When booting, CyBootloader
          will validate the image in the secondary slot and copy it to the
          primary boot slot.

    * ``--image-id`` option: Each image has its own ID. By default, secure
      image running on CM0+ core has ``--image-id 1``, non-secure image
      running on CM4 core has ``--image-id 16`` . Refer to the policy file
      for the actual ID's.

Programming the Device
======================

After building and signing, the TF-M images must be programmed into flash
memory on the PSoC64 device. There are several methods to program the images.

DAPLink mode
------------

Switch the board to DAPLink mode (refer to `Switching to DAPLink mode`_ for more
details).

Depending on the host computer settings, ``DAPLINK`` will be mounted as a
media storage device. Otherwise, mount it manually.

Copy tfm ``.hex`` files one by one to the ``DAPLINK`` device:

.. code-block:: bash

    # Replace <folder_name> with the desired name of the build folder
    BUILD_FOLDER=<folder_name>
    # Change <mount_point>
    MOUNT_POINT=<mount_point>

    cp ${BUILD_FOLDER}/bin/tfm_ns.hex ${MOUNT_POINT}/; sync
    cp ${BUILD_FOLDER}/bin/tfm_s.hex ${MOUNT_POINT}/; sync

OpenOCD
-------

Switch the board to CMSIS-DAP BULK mode (refer to
`Switching to CMSIS-DAP BULK mode`_ for more details).

Choose OpenOCD config file (``OPENOCD_TARGET_CFG``) for used board:

    * psoc6_2m_secure.cfg for CY8CKIT-064S0S2-4343W board
    * psoc6_2m_secure.cfg for CY8CKIT-064B0S2-4343W board

Optionally (if required) erase Protected Storage partition before programming
the images.

.. danger::

    Erasing Protected Storage area will clear all data and force Protected
    Storage to reformat partition.

The following command can be used to erase Protected Storage partition:

.. code-block:: bash

    # Change <openocd_path> to the path to openocd folder
    OPENOCD_PATH=<openocd_path>
    # Replace <target_cfg_file> with config file name for used board
    OPENOCD_TARGET_CFG=<target_cfg_file>
    # Note that PS_START_ADDRESS and PS_SIZE values must match PS area settings
    # from:
    #   * policy file (if CY_POLICY_CONCEPT=ON)
    #   * flash_layout.h (if CY_POLICY_CONCEPT=OFF)
    PS_START_ADDRESS=0x101c0000
    PS_SIZE=0x10000

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -f target/${OPENOCD_TARGET_CFG} \
            -c "init; reset init" \
            -c "flash erase_address ${PS_START_ADDRESS} ${PS_SIZE}" \
            -c "shutdown"

To program the signed ``tfm_s.hex`` and ``tfm_ns.hex`` images to the
device with openocd run the following commands:

.. code-block:: bash

    # Change <openocd_path> to the path to openocd folder
    OPENOCD_PATH=<openocd_path>
    # Replace <target_cfg_file> with config file name for used board
    OPENOCD_TARGET_CFG=<target_cfg_file>
    # Replace <folder_name> with the desired name of the build folder
    BUILD_FOLDER=<folder_name>

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -f target/${OPENOCD_TARGET_CFG} \
            -c "init; reset init" \
            -c "flash write_image erase ${BUILD_FOLDER}/bin/tfm_s.hex" \
            -c "shutdown"

    ${OPENOCD_PATH}/bin/openocd \
            -s ${OPENOCD_PATH}/scripts \
            -f interface/kitprog3.cfg \
            -f target/${OPENOCD_TARGET_CFG} \
            -c "init; reset init" \
            -c "flash write_image erase ${BUILD_FOLDER}/bin/tfm_ns.hex" \
            -c "reset run"

PyOCD
-----

PyOCD is installed by CySecureTools automatically. It can be used
to program TF-M images into the board.

Switch the board to DAPLink mode (refer to `Switching to DAPLink mode`_ for more
details).

Optionally (if required) erase Protected Storage partition before programming
the images.

.. danger::

    Erasing Protected Storage area will clear all data and force Protected
    Storage to reformat partition.

The following command can be used to erase Protected Storage partition:

.. code-block:: bash

    # Replace <name_of_the_board> with name of the used board (e.g. CY8CKIT-064S0S2-4343W)
    BOARD_NAME=<name_of_the_board>
    # Note that PS_START_ADDRESS and PS_SIZE values must match PS area settings
    # from:
    #   * policy file (if CY_POLICY_CONCEPT=ON)
    #   * flash_layout.h (if CY_POLICY_CONCEPT=OFF)
    PS_START_ADDRESS=0x101c0000
    PS_SIZE=0x10000

    pyocd erase -b ${BOARD_NAME} -s ${PS_START_ADDRESS}+${PS_SIZE}

To program the signed ``tfm_s.hex`` and ``tfm_ns.hex`` images to the
device with pyocd run the following commands:

.. code-block:: bash

    # Replace <name_of_the_board> with name of the used board (e.g. CY8CKIT-064S0S2-4343W)
    BOARD_NAME=<name_of_the_board>
    # Replace <folder_name> with the desired name of the build folder
    BUILD_FOLDER=<folder_name>

    pyocd flash -b ${BOARD_NAME} ${BUILD_FOLDER}/bin/tfm_s.hex

    pyocd flash -b ${BOARD_NAME} ${BUILD_FOLDER}/bin/tfm_ns.hex

**********************************************************
Using Multi-Core TF-M for Cypress PSoC64 with ModusToolBox
**********************************************************

ModusToolbox TF-M library description
=====================================

`trusted-firmware-m ModusToolbox library <https://github.com/Infineon/trusted-firmware-m>`_
provides support for TF-M in the ModusToolbox IDE. This library supports using a
prebuilt TF-M secure binary or building it from source code using CMake.

TF-M ModusToolbox library contains:

    * ``COMPONENT_<BOARD_NAME>`` folders - each
      ``COMPONENT_<BOARD_NAME>`` folder contains TF-M assets for specific
      board. These assets are:

        * ``COMPONENT_TFM_S_FW`` - contains prebuilt TF-M secure binaries.
        * ``COMPONENT_TFM_NS_INTERFACE`` - contains public interface of TF-M
          secure prebuilt binary. Allows building TF-M non-secure binary when
          prebuilt TF-M secure binary is used.

    * ``COMPONENT_TFM_S_SRC`` - allows building TF-M secure binary from
      source files in ModusToolbox using CMake.
    * ``export`` folder - when library is added to the project content of
      this folder will be exported (copied) to the project.
      ``export`` folder contains:

        * ``COMPONENT_<BOARD_NAME>`` folders - each
          ``COMPONENT_<BOARD_NAME>`` folder contains policies for specific
          board and reference keys.
        * ``reprov_helper.py`` python script - can be used to simplify
          provisioning/re-provisioning process.

Pre-requisites for Linux
========================

Build of TF-M secure binary in ModusToolbox on Linux requires following
software:

    * ``pip`` and ``venv`` python packages.
      For Ubuntu 20.04 the required python modules can be installed using
      following command:

      .. code-block:: bash

        apt install python3-venv python3-pip

Adding the library
==================

TF-M library can be added to ModusToolbox project using Library Manager or by
adding a dependency file (in ``.mtb``` format) under the ``deps/``
folder. Refer to
`ModusToolbox Library Manager user guide <https://www.infineon.com/dgdl/Infineon-ModusToolbox_Library_Manager_User_Guide_(Version_1.30)-Software-v01_00-EN.pdf?fileId=8ac78c8c7e7124d1017ed95a57ac35af&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-files>`_
for more details.

Using the library
=================

TF-M secure binary in ModusToolBox can be either used as prebuilt binary which
is shipped in ModusToolbox TF-M library or can be built from source code.

Using prebuilt TF-M secure binary
---------------------------------

To use prebuilt TF-M secure binary:

    * add ``COMPONENT_<BOARD_NAME>`` and ``COMPONENT_TFM_NS_INTERFACE``
      to the CM4 application Makefile components list.
      Example:

      .. code-block:: makefile

        COMPONENTS+=<BOARD_NAME> TFM_NS_INTERFACE

    * if RTOS is used ensure that ``COMPONENT_RTOS_AWARE`` component is
      added to the components list in CM4 project Makefile.
      Example:

      .. code-block:: makefile

        COMPONENTS+=RTOS_AWARE

    * include relevant PSA API header in your CM4 application (refer to
      `PSA API specification <https://github.com/ARM-software/psa-arch-tests/tree/master/api-specs>`_
      for more details).

Building TF-M secure binary from source code
--------------------------------------------

To build TF-M secure binary from source code:

    * create ModusToolbox project. This project will be used to build TF-M
      secure binary from source code for CM0+ core.
    * edit CM0+ project Makefile. Refer to the
      `CM0+ example makefile <https://github.com/Infineon/trusted-firmware-m/blob/master/COMPONENT_TFM_S_SRC/make/cm0p-app-example.mk>`_
      for more details.
    * edit CM4 project Makefile. Refer to the
      `CM4 example makefile <https://github.com/Infineon/trusted-firmware-m/blob/master/COMPONENT_TFM_S_SRC/make/cm4-app-example.mk>`_
      for more details.
    * edit path to TF-M Secure Application by changing ``TFM_S_APP_PATH``
      variable in CM4 application makefile. ``TFM_S_APP_PATH`` should point
      to CM0+ project.
    * optionally generate project specific policy and provide path to it in CM4
      application Makefile using ``POST_BUILD_POLICY_PATH`` variable.
    * include relevant PSA API header in your CM4 application (refer to
      `PSA API specification <https://github.com/ARM-software/psa-arch-tests/tree/master/api-specs>`_
      for more details).

.. note::

    When building TF-M from source code in ModusToolbox, the CM4 application
    uses artifacts from the CM0+ build, so the CM0+ binary should built first.
    CM0+ and CM4 applications can be built separately.

CM0+ makefile optional variables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Optional variables to configure TF-M in CM0+ Makefile:

    * ``TFM_GIT_URL`` - location of git repo with TF-M sources.
    * ``TFM_GIT_REF`` - reference to commit/branch/tag in git repo specified by
      $(TFM_GIT_URL).
    * ``TFM_PROFILE`` - TF-M profile.
    * ``TFM_ISOLATION_LEVEL`` - TF-M isolation level.
    * ``TFM_LIB_PDL`` - allows to specify path to MTB CAT1A Peripheral Driver
      library (psoc6pdl).
    * ``TFM_LIB_P64_UTILS`` - allows to specify path to PSoC64 Secure Boot
      Utilities Middleware library (p64_utils).
    * ``TFM_LIB_CY_CORE_LIB`` - allows to specify path to Cypress Core library
      (core-lib).
    * ``TFM_LIB_MBEDTLS`` - allows to specify path to Mbed TLS library
      (mbedtls).
    * ``TFM_LIB_CY_MBEDTLS_ACCELERATION`` - allows to specify path to PSoC 6
      MCUs acceleration for mbedTLS library (cy-mbedtls-acceleration).
    * ``TFM_CONFIGURE_EXT_OPTIONS`` - additional options which will be appended
      to setup CMake configuration.
    * ``TFM_BUILD_DIR`` - location of build directory
    * ``TFM_COMPILE_COMMANDS_PATH`` - location of ``compile_commands.json``
      which will be updated after CMake configuration.
    * ``TFM_CMAKE_BUILD_TYPE`` - optional parameter to specify CMake build type
      (see ``CMAKE_BUILD_TYPE`` in CMake documentation).
    * ``CONFIG`` - TF-M make file uses this variable to define
      ``CMAKE_BUILD_TYPE`` if ``TFM_CMAKE_BUILD_TYPE`` is not specified.
      Valid arguments are:

        * ``Debug`` - debug configuration (``CMAKE_BUILD_TYPE=Debug``)
        * ``Release`` - release with debug info (``CMAKE_BUILD_TYPE=RelWithDebInfo``)

***********************************
PSoC64 specific TF-M implementation
***********************************

Provisioning policies
=====================

Default PSoC64 provisioning policies were changed to suit TF-M needs. Several
sections have been modified (e.g. image slots locations and sizes) and custom
``tfm`` section was added to the policy files. For more details on TF-M
policy file fields and their usage refer to the description of
``CY_POLICY_CONCEPT`` CMake variable in
`Optional CMake configuration arguments`_ section.

``platform/ext/target/cypress/psoc64/security`` folder contains several
``COMPONENT_<BOARD_NAME>`` subfolders, each ``COMPONENT_<BOARD_NAME>``
folder contains several policy templates for the specific board. Policy
templates can be used as is or modified to suit application specific needs.

The following policy templates are provided:

    * ``policy_multi_CM0_CM4_tfm.json`` - demonstrates the usage of all TF-M
      specific policy fields. Can be used for generic applications.
    * ``policy_multi_CM0_CM4_tfm_dev_certs.json`` - extends the
      ``policy_multi_CM0_CM4_tfm.json`` with chain of trust certificates.
      Can be used when application requires certificates provisioning (e.g.
      Amazon cloud connectivity applications).
    * ``policy_multi_CM0_CM4_tfm_dev_certs_extclk.json`` - extends the
      ``policy_multi_CM0_CM4_tfm_dev_certs.json`` with external clock
      configurations. Can be used when application requires usage of external
      clocks sources.

All these policies have the RMA behavior set to erase the customer fuses, the
flash areas used for non-volatile counters and by the ITS and PS partitions,
and the non-secure image in flash. If the locations or sizes of these regions
are changed, the corresponding rma attributes should also be updated to correspond.

For more information about policies refer to the Secure Boot SDK User Guide [1]_
and PSoC64 provisioning specification [2]_.

TF-M reserved hardware resources
================================

The following peripheral resources could be occupied/configured by TF-M, make
sure there are no conflicts with non-secure application:

    * SCB5 for UART logging with P5_0, P5_1 pins for rx and tx lines accordingly.
    * TCPWM0 counter 0 for secure tests. Only used if secure tests are built.
    * TCPWM0 counter 1 for non-secure tests. Only used if non-secure tests
      are built.
    * 8-bit peripheral clock divider 1 and FLL clock are used by TF-M occupied
      peripherals. If TF-M does not occupy any peripherals then clock divider
      and FLL clock are not used.
    * IPC channels 8, 9 and 10 (and corresponding interrupts) for communication
      between non-secure (CM4) binary and secure TF-M (CM0+) binary.

Optional CMake configuration arguments
======================================

Apart from the general TF-M CMake configuration arguments that are described in
:doc:`CMake configuration TF-M document </docs/getting_started/tfm_build_instruction>`
there are several PSoC specific TF-M CMake configuration arguments:

    * Provisioned policy parsing. Can be turned ON/OFF by changing the value of
      ``CY_POLICY_CONCEPT`` CMake variable. By default
      ``CY_POLICY_CONCEPT=ON``.

      Having ``CY_POLICY_CONCEPT=ON`` allows one secure TF-M binary to
      support multiple configurations. When ``CY_POLICY_CONCEPT=ON``
      following information may/must be specified in the policy file that is
      used to provision the device:

        * Mandatory information:

            * PSoC64 build flash layout
            * initial attestation details
            * hardware version
            * whether TF-M should set the "image ok" flag

        * Optional information. If not provided TF-M will use default values:

            * watchdog timer config
            * UART settings.

              .. warning::
                In current implementation ``"uart_base"`` in policy file is
                used only for verification. Setting ``"uart_base"`` in
                policy file to values other than ``1080360960`` will result
                in a failure at run-time.

            * external clock configuration
            * debugger acquisition time
            * CM4 debug permissions

      .. warning::
        In case you are using test suites and ``CY_POLICY_CONCEPT=ON``,
        please remember that test suites are not aware of policy reading.
        So, make sure that provisioning data and values in source code
        are the same.

      Setting ``CY_POLICY_CONCEPT=OFF`` will make it so TF-M does not use
      information from provisioned policy, but instead will use values from the
      source code. The following details are used from source code:

        * flash layout from the ``flash_layout.h`` platform header file
        * initial attestation details and hardware version from the
          ``attest_hal.c`` file
        * whether TF-M should set the "image ok" flag from the
          ``tfm_hal_isolation.c`` file
        * watchdog timer and external clock configuration from the
          ``spm_hal.c`` file
        * UART settings from the ``target_cfg.c`` file

      Internally, setting ``CY_POLICY_CONCEPT`` sets several lower-level
      macros, which potentially could be enabled/disabled independently if
      needed:

        * ``CY_FLASH_LAYOUT_FROM_POLICY``
        * ``CY_ATTEST_DETAILS_FROM_POLICY``
        * ``CY_HW_VERSION_FROM_POLICY``
        * ``CY_WDT_CONFIG_FROM_POLICY``
        * ``CY_IMG_OK_CONFIG_FROM_POLICY``
        * ``CY_HW_SETTINGS_FROM_POLICY``
        * ``CY_EXTCLK_CONFIG_FROM_POLICY``

    * p64_utils heap size can be changed by changing the value of
      ``-DCY_P64_HEAP_DATA_SIZE=<value>`` CMake variable.

      By default, TF-M sets aside a block of SRAM that is large enough to
      parse the default policy provided plus a small number of additions
      to it. If the policy used to provision the device is too large to
      parse within this block, TF-M will fail to boot. In this case, the
      size of this block can be increased using this option.

    * Additional linker options can be added using
      ``-DTFM_LINK_OPTIONS=<options list>`` CMake variable. See
      `add_link_options <https://cmake.org/cmake/help/v3.15/command/add_link_options.html>`_
      for more details how to specify options for the linker.

      .. note::
        The linker files included with TF-M must be generic to handle all
        common use cases. Your project may not use every section defined in the
        linker files. In that case you may see the warnings during the build
        process using ARM clang toolchain:
        ``L6329W (pattern only matches removed unused sections).``
        In your project, you can suppress the warning by passing the
        ``-DTFM_LINK_OPTIONS=--diag_suppress=6329`` option to the linker.

    * Support of Protected Storage OEM UIDs can be turned ON/OFF by changing the
      value of ``TFM_ENABLE_PS_OEM_UID`` CMake variable. Refer to
      `Protected Storage OEM UIDs`_ section for more details on PS OEM UIDs.

Protected Storage changes
=========================

Protected Storage OEM UIDs
--------------------------

TF-M Protected Storage for PSoC64 devices was extended with optional OEM UIDs
functionality. This functionality supports retrieving provisioned policy
certificates from PSoC Flash Boot using TF-M Protected Storage service.

To retrieve provisioned certificate you can call ``tfm_ps_get()`` function
with ``uid=<ps_certificate_id>``. For details on certificate IDs refer to
the (``CY_P64_POLICY_CERTIFICATE``) section of
`p64_utils cy_p64_get_provisioning_details() function documentation <https://infineon.github.io/p64_utils/p64_utils_api_reference_manual/html/group__syscalls__api.html#ga0797f0d30bfbdbb9e18f17154f008864>`_.

.. warning::
    Certificate IDs are offset to improve PS compatibility. Use following
    code to convert Flash Boot certificate ID to TF-M Protected Storage id:

    .. code-block:: c

        ps_certificate_id = fb_certificate_id - CY_FB_CERTIFICATE_POLICY_ID_MIN + TFM_PS_OEM_UID_MIN

OEM UIDs functionality can be disabled using ``TFM_ENABLE_PS_OEM_UID`` CMake
variable. Refer to `Optional CMake configuration arguments`_ section for more
details.

Move of PS to PSA RoT
---------------------

In TF-M for PSoC devices Protected storage partition was moved from Application
to PSA Root of Trust. This change was necessary because of Protected Storage
dependency on p64_utils library global data, which is linked to PSA RoT domain.

Changing KitProg3 modes
=======================

Switching to DAPLink mode
-------------------------

To switch the board to DAPLink press the ``MODE SELECT`` button or issue the
following fw-loader command:

.. code-block:: bash

    fw-loader --mode kp3-daplink

When device is in DAPLink "Mode LED" should be slowly (1 Hz) blinking.

Switching to CMSIS-DAP BULK mode
--------------------------------

To switch the board to CMSIS-DAP BULK mode press the ``MODE SELECT`` button
or issue the following fw-loader command:

.. code-block:: bash

    fw-loader --mode kp3-bulk

When device is in CMSIS-DAP BULK "Mode LED" should be ON and not blinking.

*************************************
Porting TF-M to a custom PSoC64 board
*************************************

By default TF-M supports several reference boards (refer to
`RELEASE.md <https://github.com/Infineon/trusted-firmware-m/blob/master/RELEASE.md>`_
for the list of supported boards).

This section describes how TF-M can be ported to a custom board which is based
on one of the reference board`s MCUs.

To port TF-M to a custom board:

    1. Select reference board which is based on the same MCU as the custom board.
    2. Port policy files. To do this:

        1. Copy TF-M policy files templates from the reference board folder to
           location of your choice. More information on policy files templates
           for reference boards can be found in `Provisioning policies`_ section.
        2. Update policy files to suit the needs of your project.

    3. If peripherals assignment on the custom board is different from the
       reference board - update hardware details (e.g pins, ports, interrupt
       lines) for the peripherals that are used by TF-M. For more information on
       peripherals that are used by TF-M refer to
       `TF-M reserved hardware resources`_ section. For more information on
       hardware details locations refer to ``CY_POLICY_CONCEPT`` CMake variable
       description in `Optional CMake configuration arguments`_ section.

       .. warning::

            If ``CY_POLICY_CONCEPT`` is ``ON`` make sure to update hardware
            details in policy files.

    4. Use custom policies to provision the device and image signing. For
       more details on device provisioning refer to `Provision the board`_
       section and for more details on image signing refer to
       `Signing the images`_ section.

*********
Reference
*********

.. [1] `Secure Boot: SDK User Guide <https://www.infineon.com/dgdlac/Infineon-PSoC_64_Secure_MCU_Secure_Boot_SDK_User_Guide-Software-v07_00-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f8c361a7666&utm_source=cypress&utm_medium=referral&utm_campaign=202110_globe_en_all_integration-software>`_
.. [2] `PSoC64 provisioning specification <https://www.infineon.com/dgdl/Infineon-PSoC_64_provisioning_specification-Programming%20Specifications-v02_00-EN.pdf?fileId=8ac78c8c7ddc01d7017ddd02670f58f8>`_

--------------

*Copyright (c) 2017-2020, Arm Limited. All rights reserved.*

*Copyright (c) 2019-2022 Cypress Semiconductor Corporation (an Infineon company) or an affiliate of Cypress Semiconductor Corporation. All rights reserved.*
