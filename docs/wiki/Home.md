# StylerX Wiki

> **Note:** The GitHub wiki wasn't available, so the docs are here instead.

**StylerX** is a live theme editor plugin for OBS Studio. It lets you customize every color in OBS's UI in real time — no restarts, no file editing, no hassle.

## Pages

- [Installation](Installation.md)
- [Usage](Usage.md)
- [Theme Format](Theme-Format.md)
- [Building from Source](Building-from-Source.md)
- [CLI Reference](CLI-Reference.md)
- [Troubleshooting](Troubleshooting.md)

## What is StylerX?

A plugin that sits inside OBS as a dockable panel. It layers a Qt stylesheet on top of OBS's normal appearance so nothing gets broken. Pick colors with hex/RGB/HSV sliders, edit raw QSS with syntax highlighting, or inspect widgets to see what makes them tick.

It's written in C++17 with Qt6 and hooks into OBS through the official frontend API.

## Features at a glance

- Live color picking for 20+ UI elements
- QSS code editor with syntax highlighting and search/replace
- Widget inspector — hover to highlight, click to inspect
- Save, export, import, duplicate, and favorite themes
- Debounced stylesheet application (no flicker)
- Python CLI for install, diagnosis, and management
- Themes stored as human-readable JSON in `%APPDATA%/StylerX/themes/`
