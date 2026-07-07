# Troubleshooting

## Plugin doesn't show up in Tools menu

**Check the plugin is installed:**
```powershell
python stylerx-cli\stylerx.py doctor
```

If the doctor reports the DLL is missing, copy it again from your build output.

**Check the OBS crash log:**
OBS writes logs to `%APPDATA%\obs-studio\logs\`. Open the most recent one and search for `styler-x`. Look for:
```
[obs-plugins] Module 'styler-x' loaded successfully
```
If you see a "failed to load" message, OBS couldn't load the DLL — likely a missing dependency.

## Missing DLL / "The specified module could not be found"

The plugin depends on Qt DLLs. Run OBS from a command prompt to see which DLL is missing:

```powershell
& "C:\Program Files\obs-studio\bin\64bit\obs64.exe"
```

Then check the console output for error messages. Common missing DLLs:
- `Qt6Core.dll`, `Qt6Widgets.dll`, `Qt6Gui.dll` — make sure Qt is installed and the DLLs are accessible.
- If you built with a different Qt version than OBS uses, you may need to copy Qt DLLs into the OBS directory.

## Theme list is empty

Make sure the themes directory exists:
```powershell
python stylerx-cli\stylerx.py list
```

If the directory doesn't exist, create it:
```powershell
New-Item -ItemType Directory -Path "$env:APPDATA\StylerX\themes" -Force
```

Then the next time you open StylerX it will create a default theme.

## Colors don't change after editing

- Make sure you're editing the active theme (the one highlighted in the list)
- Click **Apply** in the Colors tab or press **Ctrl+S** in the QSS tab
- Some OBS widgets (especially ones using custom painting) don't respond to QSS — this is a limitation of OBS's Qt version, not a bug in StylerX

## Stylesheet shows "invalid" in the QSS tab

The QSS editor attempts validation. A red underline means the syntax is wrong. Check:
- Missing semicolons after property values
- Curly braces that aren't matched
- Invalid color values

## Widget inspector doesn't highlight anything

- Make sure **Enable Inspector Mode** is checked
- Some OBS child windows (popups, file dialogs) belong to a different process and can't be inspected
- The inspector only works while the StylerX dock is open

## Getting help

If you run into an issue not covered here, open an issue on the [GitHub repository](https://github.com/yummyfiles/StylerX-Styler_for_OBS_Studio/issues) with:
- Your OBS version
- Your Windows version
- The output of `python stylerx-cli\stylerx.py doctor`
- Any relevant OBS log snippets
