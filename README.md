# AMP16DX-Plugin
GoldHEN Plugin used for loading AMP16DX raw files, heavily based on the RB4DX-Plugin.
Supports v1.01 of Amplitude (2016).

## Usage

* Put this code at the bottom of `plugins.ini`
```ini
; Amplitude Deluxe Plugin
[CUSA02480]
/data/GoldHEN/plugins/AMP16DX-Plugin.prx
```

* Put processed files into `/data/GoldHEN/AMP16DX/ps4`

## Building

Install the [OpenOrbis PS4 Toolchain](https://github.com/OpenOrbis/OpenOrbis-PS4-Toolchain) and [GoldHEN Plugin SDK](https://github.com/GoldHEN/GoldHEN_Plugins_SDK), make sure the `OO_PS4_TOOLCHAIN` and `GOLDHEN_SDK` environment variables are set to their respective directories, then just type make` in the AMP16DX-Plugin project directory.
