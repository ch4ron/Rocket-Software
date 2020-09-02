AGH Space Systems Rocket Software
=================================

[![CircleCI](https://circleci.com/gh/CH4RON/Rocket-Software.svg?style=svg)](https://circleci.com/gh/CH4RON/Rocket-Software)

Boards In repositioy
--------------------

- Staszek - Main Control unit and data acquisition platform
- Kromek - Rocket engine test platform, used also for tanking and rocket ignition on launchpad
- Czapla - Communication and GPS module
- Radek - Recovery module
- Pauek - Power management board
- Ptitotek - Pitot SMU (Speed Measurement Unit)
- Arek - APRS Transciever

Building
--------

Targets

- \<board\>.elf - Build
- \<board\>_flash - Upload
- test - build and run tests
  
From CLion

https://www.jetbrains.com/help/clion/openocd-support.html

Select st_nucleo_f4.cfg as board config file

Cloning
--------
To clone with all the submodules use:

  git clone https://github.com/ch4ron/Rocket-Software --recurse-submodules

Alternatively, download them later with:

  git submodule update --init --recursive --remote
