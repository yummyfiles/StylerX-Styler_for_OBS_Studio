<!-- Banner Logo -->
<img src="assets/stylerx-logo.svg"
     alt="StylerX — Live Theme Engine for OBS Studio"
     width="100%"
     style="display:block;margin:0 auto 30px auto;" />

<h1 align="center">StylerX — make OBS look how you want</h1>

yo, this is a plugin for OBS Studio that lets you change the whole look of it in real time. every color, every panel, every button — you can tweak it all while obs is running and see the changes instantly.

---

## what is this

a live theme editor that sits inside obs as a dockable panel. it works by layering a qt stylesheet on top of obs's normal appearance. written in c++17 with qt6, hooks in through the official obs frontend api.

---

## features

- **live color picker** — 20+ ui elements you can tweak with hex input, rgb/hsv sliders, alpha, or a color dialog. all sync in real time.
- **raw qss editor** — full code editor with syntax highlighting, search/replace, and ctrl+s to apply. lets you do border radii, gradients, font sizes, etc.
- **widget inspector** — hover any obs element to see its class name, stylesheet, and colors. click to jump straight to editing.
- **save/export/import** — themes are json files. save, duplicate, delete, favorite, rename, export, import.
- **debounced updates** — no flickering while you drag sliders. configurable or instant.
- **persistent dock** — remembers position and visibility across obs restarts.

---

## how is this different from obs themes

1. **live editing** — change a color, see it instantly. no restarting obs.
2. **no file editing** — all in the ui, no digging through obs install folders.
3. **per-user** — themes stored in your appdata, not system-wide.
4. **import/export** — share themes as single json files.
5. **widget inspector** — obs doesnt have anything like this.

---

## the cli

comes with a python cli for managing things outside of obs:

```
stylerx install        copies the dll to obs plugins folder
stylerx doctor         checks if everything is set up right
stylerx version        shows version info
stylerx logo           prints the ascii logo
stylerx help           lists all commands
```

---

## installing the plugin

### manual way

```powershell
cd path\to\StylerX-Styler_for_OBS_Studio
copy "StylerX\build\Release\styler-x.dll" "%ProgramFiles%\obs-studio\obs-plugins\64bit\"
```

> **note:** if you don't have admin rights, copy to `%APPDATA%\obs-studio\plugins\64bit\` instead.

### cli way (recommended)

```powershell
cd path\to\StylerX-Styler_for_OBS_Studio
python stylerx-cli\stylerx.py install --build-dir StylerX\build
```

the cli will try `%ProgramFiles%` first, fall back to `%APPDATA%` if admin isnt available, and create the themes directory automatically.

then launch obs and go to **Tools → StylerX Studio**.

---

## building from source

```powershell
git clone https://github.com/yummyfiles/StylerX-Styler_for_OBS_Studio.git
cd StylerX-Styler_for_OBS_Studio

# download windows-deps-qt6-*-x64.zip from https://github.com/obsproject/obs-deps/releases
# extract to StylerX/deps/qt6/

cd StylerX
mkdir build -Force
cd build
cmake .. -DCMAKE_PREFIX_PATH="../deps/qt6" -Dlibobs_DIR="../deps/obs-sdk/cmake"
cmake --build . --config Release
```

the built dll will be at `StylerX/build/Release/styler-x.dll`.

### requirements

| thing | version |
|-------|---------|
| obs studio | 32.1.2+ |
| vs build tools 2022 | msvc 19.44+ |
| cmake | 3.22+ |
| qt6 | 6.11.1 (from obs-deps) |
| windows sdk | 10.0.26100+ |

---

## where themes are stored

```
%APPDATA%/StylerX/themes/
```

drop json theme files in there and they'll appear automatically in StylerX Studio.

---

## license

gnu general public license v2.0 — do what you want with it, just keep it open source.
