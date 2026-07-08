<!-- Banner Logo -->
<img src="assets/stylerx-logo.svg"
     alt="StylerX — Live Theme Engine for OBS Studio"
     width="100%"
     style="display:block;margin:0 auto 30px auto;" />

<!-- Typing Demo -->
<img src="assets/typing.svg"
     alt="StylerX CLI typing demo"
     width="600"
     style="display:block;margin:40px auto 30px auto;" />

<!-- Badges -->
<p align="center">
  <a href="docs/wiki/Home.md"><img src="https://img.shields.io/badge/Wiki-Home-0078d4?style=flat-square" alt="Wiki"></a>
  <a href="docs/wiki/Installation.md"><img src="https://img.shields.io/badge/Installation-Get_Started-2ea44f?style=flat-square" alt="Installation"></a>
  <a href="docs/wiki/Building-from-Source.md"><img src="https://img.shields.io/badge/Building-From_Source-d47500?style=flat-square" alt="Building"></a>
  <a href="docs/wiki/Usage.md"><img src="https://img.shields.io/badge/Usage-Guide-8250df?style=flat-square" alt="Usage"></a>
  <a href="docs/wiki/Theme-Format.md"><img src="https://img.shields.io/badge/Theme_Format-Reference-1b9aaa?style=flat-square" alt="Theme Format"></a>
  <a href="docs/wiki/CLI-Reference.md"><img src="https://img.shields.io/badge/CLI-Reference-6e7681?style=flat-square" alt="CLI"></a>
  <a href="docs/wiki/Troubleshooting.md"><img src="https://img.shields.io/badge/Troubleshooting-Help-cf222e?style=flat-square" alt="Troubleshooting"></a>
</p>

<h1 align="center">StylerX — make OBS look how you want</h1>

yo, this is a plugin for OBS Studio that lets you change the whole look of it in real time. every color, every panel, every button — you can tweak it all while obs is running and see the changes instantly. works by layering a qt stylesheet on top of obs — nothing gets broken or overwritten. written in c++17 with qt6.

---

## what you can do with it

- **live color picker** — 20+ ui elements (background, panels, buttons, text, accent, borders, scrollbars, etc.) with hex input, rgb/hsv sliders, alpha, or a color dialog. all sync in real time.
- **raw qss editor** — full code editor with syntax highlighting, search/replace, and ctrl+s to apply. lets you do border radii, gradients, font sizes, etc.
- **widget inspector** — hover any obs element to see its class name, stylesheet, and colors. click to jump straight to editing.
- **save/export/import** — themes are json files. save, duplicate, delete, favorite, rename, export, import, search.
- **debounced updates** — no flickering while you drag sliders. configurable or instant.
- **persistent dock** — remembers position and visibility across obs restarts.

also comes with a python cli (`stylerx-cli/stylerx.py`):

```
python stylerx-cli\stylerx.py install     Install the plugin
python stylerx-cli\stylerx.py uninstall   Remove the plugin
python stylerx-cli\stylerx.py update      Check for updates
python stylerx-cli\stylerx.py doctor      Check if everything is set up right
python stylerx-cli\stylerx.py version     Show version info
python stylerx-cli\stylerx.py logo        Print the logo
python stylerx-cli\stylerx.py help        Show available commands
```

---

## requirements

| thing | version |
|-------|---------|
| obs studio | 32.1.2+ |
| vs build tools 2022 | msvc 19.44+ |
| cmake | 3.22+ |
| qt6 | 6.11.1 (from obs-deps) |
| windows sdk | 10.0.26100+ |

---

## installing the plugin

first, get the repo — either clone it or [download the zip](https://github.com/yummyfiles/StylerX-Styler_for_OBS_Studio/archive/refs/heads/main.zip). if you downloaded the zip, extract it first. then cd into the folder:

```powershell
cd C:\Users\yourname\Downloads\StylerX-Styler_for_OBS_Studio-main\StylerX-Styler_for_OBS_Studio-main
```

then run the installer:

```powershell
python stylerx-cli\stylerx.py install --build-dir StylerX\build
```

the cli will try `%ProgramFiles%` first, fall back to `%APPDATA%` if admin isnt available, and create the themes directory automatically.

then launch obs and go to **Tools → StylerX Studio**.

---

## where themes are stored

```
%APPDATA%/StylerX/themes/
```

drop json theme files in there and they'll appear automatically in StylerX Studio.

---

## license

gnu general public license v2.0 — do what you want with it, just keep it open source.
