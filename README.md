<div align="center">

```
  ██╗    ███████╗████████╗██╗   ██╗██╗     ███████╗██████╗ ██╗  ██╗    ██╗
 ██╔╝    ██╔════╝╚══██╔══╝╚██╗ ██╔╝██║     ██╔════╝██╔══██╗╚██╗██╔╝    ╚██╗
██╔╝     ███████╗   ██║    ╚████╔╝ ██║     █████╗  ██████╔╝ ╚███╔╝      ╚██╗
╚██╗     ╚════██║   ██║     ╚██╔╝  ██║     ██╔══╝  ██╔══██╗ ██╔██╗      ██╔╝
 ╚██╗    ███████║   ██║      ██║   ███████╗███████╗██║  ██║██╔╝ ██╗    ██╔╝
  ╚═╝    ╚══════╝   ╚═╝      ╚═╝   ╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝    ╚═╝

</div>

# StylerX — Live Theme Engine for OBS Studio

<div align="center">

[![OBS](https://img.shields.io/badge/OBS-32.1.2-302E31?logo=obsstudio&logoColor=white)](https://obsproject.com)
[![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org)
[![Qt](https://img.shields.io/badge/Qt-6-41CD52?logo=qt&logoColor=white)](https://qt.io)
[![CMake](https://img.shields.io/badge/CMake-3.22+-064F8C?logo=cmake&logoColor=white)](https://cmake.org)
[![Python](https://img.shields.io/badge/Python-3-3776AB?logo=python&logoColor=white)](https://python.org)
[![License](https://img.shields.io/badge/License-GPLv2-blue)](LICENSE)
[![PRs](https://img.shields.io/badge/PRs-welcome-brightgreen)](https://github.com/yummyfiles/StylerX-Styler_for_OBS_Studio/pulls)

**Style your OBS. Live.**

</div>

Create, customize, and preview OBS Studio themes in real time. StylerX layers a user-defined Qt stylesheet on top of OBS's base appearance — tweak colors, edit raw QSS, and inspect widgets from a docked panel inside OBS.

---

## ✨ Features

<table>
<tr>
<td width="50%">

### 🎨 Live Color Picker
Adjust 20+ UI colors — backgrounds, buttons, accents, text, tabs, scrollbars — with hex inputs, RGB/A sliders, or HSV wheels. Changes preview instantly.

### 📝 QSS Editor
Full-featured code editor with syntax highlighting, search/replace, line numbers, and <kbd>Ctrl+S</kbd> to apply.

### 🔍 Widget Inspector
Hover to highlight any OBS widget, click to inspect its stylesheet, class name, and object name.

</td>
<td width="50%">

### 💾 Theme Library
Save, duplicate, rename, favorite, export, and import themes as JSON. Stored in `%APPDATA%/StylerX/themes/`.

### ⚡ Debounced Apply
Color updates are debounced (configurable) so bulk edits don't cause flicker.

### 🖥️ CLI Companion
Command-line tool for installing, diagnosing, and managing the plugin.

</td>
</tr>
</table>

---

## 📋 Requirements

| Component | Version |
|-----------|---------|
| [OBS Studio](https://obsproject.com) | 32.1.2+ |
| Windows SDK | 10.0.26100+ |
| [Visual Studio 2022 Build Tools](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022) | MSVC 19.44+ |
| [CMake](https://cmake.org) | 3.22+ |
| [Qt](https://qt.io) | 6.x (from [obs-deps](https://github.com/obsproject/obs-deps)) |

---

## 🔧 Build

```powershell
# 1. Clone
git clone https://github.com/yummyfiles/StylerX-Styler_for_OBS_Studio.git
cd StylerX-Styler_for_OBS_Studio

# 2. Download Qt6 dev package for Windows from obs-deps
#    Extract to: StylerX/deps/qt6/

# 3. Configure
cd StylerX
mkdir build -Force; cd build
cmake .. -DCMAKE_PREFIX_PATH="../deps/qt6" -Dlibobs_DIR="../deps/obs-sdk/cmake"

# 4. Build
cmake --build . --config Release
```

> **Note:** The OBS SDK headers and CMake config are included in the repo under `StylerX/deps/obs-sdk/`. The Qt6 package is **not** tracked — download it separately from [obs-deps releases](https://github.com/obsproject/obs-deps/releases) (`windows-deps-qt6-*-x64.zip`).

---

## 📦 Install

### Manual
```powershell
copy "StylerX\build\Release\styler-x.dll" "%ProgramFiles%\obs-studio\obs-plugins\64bit\"
copy "StylerX\build\Release\styler-x.pdb" "%ProgramFiles%\obs-studio\obs-plugins\64bit\"
```

### Via CLI
```powershell
python stylerx-cli\stylerx.py install --build-dir StylerX\build
```

### Verify Installation
```powershell
python stylerx-cli\stylerx.py doctor
```

---

## 🚀 Usage

1. Launch **OBS Studio**.
2. Go to **Tools → StylerX Studio**.
3. The dock panel appears with three tabs:

<div align="center">

| Tab | Description |
|-----|-------------|
| 🎨 **Colors** | Pick and edit 20+ theme colors in real time |
| 📝 **QSS** | Raw stylesheet editor with syntax highlighting |
| 🔍 **Inspector** | Hover to highlight, click to inspect OBS widgets |

</div>

---

## 🗂️ Theme Storage

Themes are JSON files stored at:

```
%APPDATA%/StylerX/themes/
```

Each theme includes metadata and a color map:

```json
{
  "format_version": 1,
  "name": "My Theme",
  "author": "you",
  "created": "2026-07-06T17:00:00Z",
  "modified": "2026-07-06T17:30:00Z",
  "colors": {
    "background": "#1e1e1e",
    "accent": "#0078d4",
    "text": "#cccccc"
  }
}
```

---

## 🖥️ CLI Reference

```
python stylerx-cli\stylerx.py install    Copy plugin DLL to OBS plugins folder
python stylerx-cli\stylerx.py doctor     Check OBS + plugin installation status
python stylerx-cli\stylerx.py version    Show CLI and plugin versions
python stylerx-cli\stylerx.py logo       Print the StylerX ASCII logo
python stylerx-cli\stylerx.py help       Show available commands
```

---

## 📁 Project Structure

```
StylerX/
├── CMakeLists.txt                    CMake build configuration
├── deps/
│   ├── obs-sdk/                      OBS Studio SDK headers & cmake config
│   └── qt6/                          Qt6 dev package (download, not tracked)
├── build/                            Build output (not tracked)
└── src/
    ├── plugin.cpp                    OBS module entry point
    ├── ThemeManager.h/.cpp           Theme lifecycle & current theme state
    ├── ThemeLibraryManager.h/.cpp    Disk read/write for user themes
    ├── ThemeApplier.h/.cpp           Apply stylesheets with debouncing
    ├── ThemeStorage.h/.cpp           JSON serialization (ThemeData ↔ JSON)
    ├── StyleParser.h/.cpp            ThemeData → QSS stylesheet
    ├── ThemeData.h                   Theme struct with versioning & metadata
    ├── SettingsManager.h/.cpp        Persist dock geometry & visibility
    ├── ColorPickerWidget.h/.cpp      Color editor (RGB, HSV, Hex, alpha)
    ├── QSSEditorWidget.h/.cpp        Syntax-highlighted QSS code editor
    ├── WidgetInspector.h/.cpp        Hover highlight & widget inspection
    └── ThemeEditorDock.h/.cpp        Main dock panel UI (3 tabs)

stylerx-cli/
├── stylerx.py                        Python CLI for install/doctor/version
├── stylerx.bat                       Windows batch launcher
└── README.md                         CLI-specific documentation
```

---

## 🤝 Contributing

Pull requests are welcome! Feel free to open issues for bugs, feature requests, or questions.

---

## 📄 License

GNU General Public License v2.0. See [LICENSE](LICENSE) for details.
