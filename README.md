AGH Space Systems Rocket Software
================================

[![CircleCI](https://circleci.com/gh/CH4RON/Rocket-Software.svg?style=svg)](https://circleci.com/gh/CH4RON/Rocket-Software)

Boards In repositioy
--------------------

- Staszek - Main Control unit and data acquisition platform
- Kromek - Rocket engine test platform, used also for tanking and rocket ignition on launchpad
- Czapla - Communication and GPS module
- Radek - Recovery module
- Pauek - Power management board


Building
--------

Using scripts

    Build scripts use python 2.7

    Usage: build.py [OPTIONS] BOARD COMMAND [ARGS]...

    Commands:
      build     Build target for board
      clean     Clean all targets for board
      flash     Build and flash target
      purge     Purge all targets for board
      simulate  Run test on simulator for board
      test      Run tests on board

Using Clion
    
    Import project as STM32CubeMX project
    
    Clion uses automatic CMakeLists.txt generation from template. 
    If CMakeLists.txt is overwritten, revert changes.

    In File/Settings/Build, Execution, Deployment/CMake
    Create profiles with Build types:

    Debug
    Simulate
    Simulate-test
    Test
