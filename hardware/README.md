# Hardware — ESP32-S3 Automotive Simulator

This directory contains the hardware documentation for the ESP32-S3 Automotive Simulator, an educational automotive electronics prototype built on a breadboard.

## Hardware Overview

The system uses a YD-ESP32-S3 development board with an ESP32-S3-WROOM-1-N16R8 module.

Implemented hardware includes:

- Ambient light sensing using a photoresistor
- Five push buttons for vehicle controls and event simulation
- Headlight, brake light, and turn signal LEDs
- Airbag deployment indicator
- Five 2N7000 MOSFET output stages
- Onboard addressable RGB LED for engine status indication

## Power Supply

The ESP32-S3 development board is powered through USB.

External LEDs are powered by a separate regulated 3.3 V supply, represented by the `+3V3_EXT` net.

Both supplies share a common ground. Their positive 3.3 V rails must remain separate.

## Schematic

The KiCad project is located in [`kicad/`](kicad/).

A PDF version of the schematic is available in the [documentation directory](../docs/automotive-schematic.pdf).

The schematic uses a project-specific symbol representing the complete ESP32-S3 development board.

## Design Status

- Schematic revision: 1.0
- Hardware implementation: Breadboard prototype
- Electrical Rules Check: 0 errors, 0 warnings
- PCB layout: Not implemented

The schematic documents the educational prototype and is not intended as a production-ready automotive electrical design.
