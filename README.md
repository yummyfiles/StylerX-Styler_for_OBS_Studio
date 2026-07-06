<div align="center">

<pre style="background:transparent">
  ██╗    ███████╗████████╗██╗   ██╗██╗     ███████╗██████╗ ██╗  ██╗    ██╗
 ██╔╝    ██╔════╝╚══██╔══╝╚██╗ ██╔╝██║     ██╔════╝██╔══██╗╚██╗██╔╝    ╚██╗
██╔╝     ███████╗   ██║    ╚████╔╝ ██║     █████╗  ██████╔╝ ╚███╔╝      ╚██╗
╚██╗     ╚════██║   ██║     ╚██╔╝  ██║     ██╔══╝  ██╔══██╗ ██╔██╗      ██╔╝
 ╚██╗    ███████║   ██║      ██║   ███████╗███████╗██║  ██║██╔╝ ██╗    ██╔╝
  ╚═╝    ╚══════╝   ╚═╝      ╚═╝   ╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝    ╚═╝
</pre>

</div>

# StylerX — make OBS look how you want

yo, this is a plugin for OBS Studio that lets you change the whole look of it in real time. no more being stuck with the same dark theme forever.

- **pick colors live** — backgrounds, buttons, text, accents, all of it. hex, rgb, hsv, whatever.
- **qss editor** — if you know css, you know this. syntax highlighting, search/replace, ctrl+s to apply.
- **widget inspector** — hover over anything in obs and see what it is, whats its stylesheet looks like.
- **save your themes** — they live in `%APPDATA%/StylerX/themes/` as json. export, import, duplicate, whatever.

it just layers on top of obs's normal look so nothing breaks.

---

## Install

grab the dll from releases or build it yourself, then:

```powershell
copy "StylerX\build\Release\styler-x.dll" "%ProgramFiles%\obs-studio\obs-plugins\64bit\"
copy "StylerX\build\Release\styler-x.pdb" "%ProgramFiles%\obs-studio\obs-plugins\64bit\"
```

or use the cli:

```powershell
python stylerx-cli\stylerx.py install --build-dir StylerX\build
```

then open obs and go to **Tools → StylerX Studio**.

---

## cli

```
python stylerx-cli\stylerx.py install    copies the dll to obs
python stylerx-cli\stylerx.py doctor     checks if everything is set up right
python stylerx-cli\stylerx.py version    shows versions
python stylerx-cli\stylerx.py logo       prints the ascii logo
```

---

*licensed under gplv2*
