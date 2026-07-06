# StylerX - Live Theme Engine for OBS Studio

Create, customize, and preview OBS Studio themes in real time. StylerX layers a user-defined Qt stylesheet on top of OBS's base appearance, letting you tweak colors, edit raw QSS, and inspect widgets — all from a docked panel inside OBS.

![StylerX Logo](https://raw.githubusercontent.com/yummyfiles/StylerX-Styler_for_OBS_Studio/main/stylerx-cli/stylerx.png)

## Features

- **Live color picking** — Adjust 20+ UI colors (backgrounds, buttons, accents, text, tabs, scrollbars, etc.) with hex inputs, RGB/A sliders, or HSV wheels. Changes preview instantly.
- **QSS Editor** — Full-featured code editor with syntax highlighting, search/replace, line numbers, and Ctrl+S apply.
- **Widget Inspector** — Hover to highlight any widget, click to inspect its stylesheet, class name, and object name.
- **Theme Library** — Save, duplicate, rename, favorite, export, and import themes as JSON files. Themes stored in `%APPDATA%/StylerX/themes/`.
- **Debounced apply** — Color updates are debounced (configurable) so bulk edits don't cause flicker.
- **CLI companion** — StylerX CLI (`stylerx.py`) provides `install`, `doctor`, `version`, `logo`, and `help` commands for managing the plugin.

## Requirements

| Component | Version |
|-----------|---------|
| OBS Studio | 32.1.2+ |
| Windows SDK | 10.0.26100+ |
| Visual Studio 2022 Build Tools | MSVC 19.44+ |
| CMake | 3.22+ |
| Qt | 6.x (from obs-deps) |

## Build

```powershell
# Clone
git clone https://github.com/yummyfiles/StylerX-Styler_for_OBS_Studio.git
cd StylerX-Styler_for_OBS_Studio

# Download Qt6 dev package (obs-deps)
# Extract to StylerX/deps/qt6/

# Configure
cd StylerX
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="../deps/qt6" -Dlibobs_DIR="../deps/obs-sdk/cmake"

# Build
cmake --build . --config Release
```

## Install

### Manual
```powershell
copy "build\Release\styler-x.dll" "%ProgramFiles%\obs-studio\obs-plugins\64bit\"
copy "build\Release\styler-x.pdb" "%ProgramFiles%\obs-studio\obs-plugins\64bit\"
```

### Via CLI
```powershell
python stylerx-cli\stylerx.py install --build-dir StylerX\build
```

## Usage

1. Launch OBS Studio.
2. Go to **Tools → StylerX Studio**.
3. The dock will appear with three tabs:
   - **Colors** — Pick and edit theme colors.
   - **QSS** — Raw stylesheet editor with syntax highlighting.
   - **Inspector** — Inspect OBS widget properties.

## Theme Storage

User themes are stored as JSON files in:
```
%APPDATA%/StylerX/themes/
```

Each theme includes metadata (`format_version`, `author`, `created`, `modified`) plus a `colors` map of CSS variable names to hex values.

## CLI Commands

```powershell
python stylerx-cli\stylerx.py install    Copy plugin DLL to OBS
python stylerx-cli\stylerx.py doctor     Check OBS/plugin installation
python stylerx-cli\stylerx.py version    Show CLI and plugin versions
python stylerx-cli\stylerx.py logo       Print ASCII logo
python stylerx-cli\stylerx.py help       Show help
```

## Project Structure

```
StylerX/
├── CMakeLists.txt              # CMake build configuration
├── deps/
│   ├── obs-sdk/                # OBS Studio SDK headers & cmake config
│   └── qt6/                    # Qt6 development package (extracted, not tracked)
├── build/                      # Build output (not tracked)
└── src/
    ├── plugin.cpp              # OBS module entry point
    ├── ThemeManager.h/.cpp     # Theme lifecycle & current theme state
    ├── ThemeLibraryManager.h/.cpp  # Disk read/write for user themes
    ├── ThemeApplier.h/.cpp     # Applies stylesheets with debouncing
    ├── ThemeStorage.h/.cpp     # JSON serialization
    ├── StyleParser.h/.cpp      # ThemeData → QSS conversion
    ├── ThemeData.h             # Theme struct with versioning
    ├── SettingsManager.h/.cpp  # Persist dock geometry & visibility
    ├── ColorPickerWidget.h/.cpp    # Color editor with RGB/HSV/Hex
    ├── QSSEditorWidget.h/.cpp  # Syntax-highlighted QSS code editor
    ├── WidgetInspector.h/.cpp  # Hover & inspect OBS widgets
    └── ThemeEditorDock.h/.cpp  # Main dock panel UI

stylerx-cli/
├── stylerx.py                  # Python CLI for install/doctor/version
├── stylerx.bat                 # Windows batch launcher
└── README.md                   # CLI-specific docs
```

## License

GNU General Public License v2.0
