# AMP16DX-Plugin
GoldHEN Plugin used for loading AMP16DX files, heavily based on the RB4DX-Plugin (1.0 debug fork).
Supports v1.00 and v1.01 of Amlitude (2016).

## Usage

* Put this code at the bottom of `plugins.ini`
```ini
; Amplitude Deluxe Plugin
[CUSA02480]
/data/GoldHEN/plugins/AMP16DX-Plugin.prx
```

* Put processed files into `/data/GoldHEN/AMP16DX/ps4`

## Extra Features

* Make a file in `/data/GoldHEN/AMP16DX` called `writelog.ini`.
This will write the values of Symbol::Symbol to `/data/GoldHEN/Amp16SymbolLog.txt`

## Building

Install the [OpenOrbis PS4 Toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain) and [GoldHEN Plugin SDK](https://github.com/GoldHEN/GoldHEN_Plugins_SDK), make sure the `OO_PS4_TOOLCHAIN` and `GOLDHEN_SDK` environment variables are set to their respective directories, then just type `make` in the AMP16DX-Plugin project directory.

## Included Dependencies

[inih](https://github.com/benhoyt/inih) - ini parser
