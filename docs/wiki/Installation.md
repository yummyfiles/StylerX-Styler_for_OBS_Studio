# Installation

## Prerequisites

- OBS Studio 32.1.2 or newer installed at `C:\Program Files\obs-studio`

## Manual install

After building the plugin, copy the output to OBS:

```powershell
copy "StylerX\build\Release\styler-x.dll" "%ProgramFiles%\obs-studio\obs-plugins\64bit\"
copy "StylerX\build\Release\styler-x.pdb" "%ProgramFiles%\obs-studio\obs-plugins\64bit\"
```

## CLI install

If you have the Python CLI set up:

```powershell
python stylerx-cli\stylerx.py install --build-dir StylerX\build
```

## Verifying it worked

Run the doctor command:

```powershell
python stylerx-cli\stylerx.py doctor
```

This checks:
1. OBS is installed at the expected path
2. The plugin DLL exists in the plugins folder
3. The themes directory exists in AppData

## Post-install

Launch OBS Studio and go to **Tools → StylerX Studio** to open the theme editor dock.
