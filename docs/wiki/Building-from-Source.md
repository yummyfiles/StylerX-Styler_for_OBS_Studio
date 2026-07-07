# Building from Source

## Prerequisites

- [Visual Studio 2022](https://visualstudio.microsoft.com/) with Desktop development with C++ workload
- [CMake](https://cmake.org/download/) 3.20+
- [Qt 6.6+](https://www.qt.io/download/) — install the MSVC 2022 64-bit version
- [OBS Studio](https://obsproject.com/) 32.1.2+ (installed, for headers and libs)
- [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)
- [Python](https://python.org/) 3.10+ (for the CLI)
- [Git](https://git-scm.com/)

## Verify your environment

```powershell
python stylerx-cli\stylerx.py doctor
```

This checks that OBS is installed and the build tools are available.

## Build

From the repo root:

```powershell
cd StylerX
cmake --preset default
cmake --build build --config Release
```

The output will be in `StylerX\build\Release\`:
- `styler-x.dll` — the OBS plugin
- `styler-x.pdb` — debug symbols (optional)

## Install

```powershell
copy "build\Release\styler-x.dll" "%ProgramFiles%\obs-studio\obs-plugins\64bit\"
```

Or use the CLI:

```powershell
python ..\stylerx-cli\stylerx.py install --build-dir build
```

## Build notes

- The CMake preset expects OBS at `C:\Program Files\obs-studio`. If yours is elsewhere, override `OBS_PATH` in CMakePresets.json.
- Qt must be locatable by CMake's `find_package`. If CMake can't find it, set `CMAKE_PREFIX_PATH` to your Qt install directory.
- Only x64 (64-bit) builds are supported.
- Debug builds work but are slower due to the stylesheet debounce — use Release.
