# CLI Reference

The Python CLI `stylerx-cli\stylerx.py` helps with installation and diagnostics.

## Usage

```powershell
python stylerx-cli\stylerx.py <command> [options]
```

## Commands

### `doctor`

Check the system for all StylerX requirements.

```powershell
python stylerx-cli\stylerx.py doctor
```

Checks performed:
1. OBS exists at `C:\Program Files\obs-studio`
2. Plugin DLL exists in the plugins folder
3. Themes directory exists in AppData
4. Windows version compatibility

Returns a pass/fail status for each check.

### `install`

Copy the built plugin to the OBS plugins folder.

```powershell
python stylerx-cli\stylerx.py install [--build-dir <path>]
```

| Option | Default | Description |
|--------|---------|-------------|
| `--build-dir` | `StylerX\build` | Path to the CMake build output directory |

The command copies `styler-x.dll` (and `styler-x.pdb` if present) from `<build-dir>\Release\` to the OBS plugins folder. Requires administrator privileges.

### `list`

List installed themes.

```powershell
python stylerx-cli\stylerx.py list
```

Prints every theme found in the themes directory with its name, author, and read-only status.

## Exit codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | Error (missing dependency, build failure, etc.) |
