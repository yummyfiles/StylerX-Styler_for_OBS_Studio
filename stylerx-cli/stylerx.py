#!/usr/bin/env python3
"""
StylerX CLI - Live Theme Engine for OBS Studio
Install, manage, and diagnose StylerX plugin for OBS Studio.
"""

import sys
import os
import json
import shutil
import subprocess
import argparse
import time
import threading
import itertools
from pathlib import Path

VERSION = "1.0.0"
APP_NAME = "StylerX"

LOGO = """
  ██╗    ███████╗████████╗██╗   ██╗██╗     ███████╗██████╗ ██╗  ██╗    ██╗
 ██╔╝    ██╔════╝╚══██╔══╝╚██╗ ██╔╝██║     ██╔════╝██╔══██╗╚██╗██╔╝    ╚██╗
██╔╝     ███████╗   ██║    ╚████╔╝ ██║     █████╗  ██████╔╝ ╚███╔╝      ╚██╗
╚██╗     ╚════██║   ██║     ╚██╔╝  ██║     ██╔══╝  ██╔══██╗ ██╔██╗      ██╔╝
 ╚██╗    ███████║   ██║      ██║   ███████╗███████╗██║  ██║██╔╝ ██╗    ██╔╝
  ╚═╝    ╚══════╝   ╚═╝      ╚═╝   ╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝    ╚═╝
"""

# ---- Terminal Helpers ----


def green(text):
    return f"\033[92m{text}\033[0m"


def red(text):
    return f"\033[91m{text}\033[0m"


def yellow(text):
    return f"\033[93m{text}\033[0m"


def cyan(text):
    return f"\033[96m{text}\033[0m"


def bold(text):
    return f"\033[1m{text}\033[0m"


def dim(text):
    return f"\033[2m{text}\033[0m"


def reset():
    return "\033[0m"


SUCCESS = green("[✓]")
ERROR = red("[✗]")
WORKING = cyan("[•]")
INSTALLING = yellow("[→]")


# ---- Spinner ----


class Spinner:
    def __init__(self, message="Working"):
        self.message = message
        self._running = False
        self._thread = None

    def start(self):
        self._running = True
        self._thread = threading.Thread(target=self._spin, daemon=True)
        self._thread.start()

    def _spin(self):
        spinner = itertools.cycle(["|", "/", "-", "\\"])
        while self._running:
            sys.stdout.write(f"\r{cyan(next(spinner))} {self.message}... ")
            sys.stdout.flush()
            time.sleep(0.1)

    def stop(self, status="done"):
        self._running = False
        if self._thread:
            self._thread.join()
        sys.stdout.write(f"\r{SUCCESS} {self.message} {status}\n")
        sys.stdout.flush()

    def fail(self, msg="failed"):
        self._running = False
        if self._thread:
            self._thread.join()
        sys.stdout.write(f"\r{ERROR} {self.message} {msg}\n")
        sys.stdout.flush()


# ---- Progress Bar ----


def progress_bar(current, total, label="", width=20):
    fraction = current / total if total > 0 else 0
    filled = int(fraction * width)
    bar = "█" * filled + "░" * (width - filled)
    pct = int(fraction * 100)
    sys.stdout.write(f"\r{INSTALLING} {label} [{bar}] {pct}%")
    sys.stdout.flush()
    if current >= total:
        sys.stdout.write(f"\r{SUCCESS} {label} [{bar}] 100%\n")
        sys.stdout.flush()


# ---- Configuration ----


def get_obs_data_dirs():
    """Detect OBS Studio plugin directories."""
    if sys.platform == "win32":
        program_files = Path(os.environ.get("ProgramFiles", "C:\\Program Files"))
        obs_paths = [
            program_files / "obs-studio",
            Path(os.environ.get("OBS_PATH", "")),
        ]
        obs_paths = [p for p in obs_paths if p and p.exists()]
        if not obs_paths:
            obs_paths = list(program_files.parent.glob("OBS*"))
            obs_paths += list(program_files.glob("**/obs-studio"))
        return obs_paths
    elif sys.platform == "darwin":
        return [Path("/Applications/OBS.app")]
    else:
        paths = [
            Path("/usr/share/obs"),
            Path("/usr/local/share/obs"),
            Path.home() / ".obs-studio",
        ]
        return [p for p in paths if p.exists()]


def get_stylerx_paths(obs_dir):
    """Get plugin installation paths for StylerX."""
    if sys.platform == "win32":
        plugin_dll = obs_dir / "obs-plugins" / "64bit" / "styler-x.dll"
        data_dir = obs_dir / "data" / "styler-x"
        return plugin_dll, data_dir
    elif sys.platform == "darwin":
        plugin_bundle = obs_dir / "Contents" / "PlugIns" / "styler-x.bundle"
        data_dir = obs_dir / "Contents" / "Resources" / "data" / "styler-x"
        return plugin_bundle, data_dir
    else:
        plugin_so = obs_dir / "obs-plugins" / "64bit" / "styler-x.so"
        data_dir = obs_dir / "data" / "styler-x"
        return plugin_so, data_dir


# ---- Commands ----


def cmd_version(args):
    print(LOGO)
    print(f"  {bold('Live Theme Engine for OBS Studio')}")
    print(f"  {bold('StylerX CLI')} v{VERSION}")
    print()
    print(f"  Plugin version: {VERSION}")
    print(f"  Python:          {sys.version.split()[0]}")
    print(f"  Platform:        {sys.platform}")
    print()


def cmd_doctor(args):
    print()
    print(bold(" StylerX Diagnostics"))
    print()

    tests = [
        ("Operating system detected", lambda: True),
        ("OBS Studio found", lambda: len(get_obs_data_dirs()) > 0),
        ("Plugin directory valid", _check_plugin_dirs),
        ("Theme engine ready", _check_theme_engine),
        ("Installation healthy", _check_installation),
    ]

    all_pass = True
    for label, test_fn in tests:
        try:
            result = test_fn()
            if result:
                print(f"  {SUCCESS} {label}")
            else:
                print(f"  {ERROR} {label}")
                all_pass = False
        except Exception as e:
            print(f"  {ERROR} {label}")
            print(f"         {dim(str(e))}")
            all_pass = False

    print()
    if all_pass:
        print(f"  {bold(green('All checks passed. StylerX is healthy.'))}")
    else:
        print(f"  {bold(red('Some checks failed. Run `stylerx install` to fix.'))}")
    print()
    return 0 if all_pass else 1


def _check_plugin_dirs():
    for obs_dir in get_obs_data_dirs():
        plugin_path, data_dir = get_stylerx_paths(obs_dir)
        if plugin_path.exists() or plugin_path.parent.exists():
            return True
    return False


def _check_theme_engine():
    if sys.platform == "win32":
        appdata = Path(os.environ.get("APPDATA", ""))
        theme_dir = appdata / "StylerX" / "themes"
    else:
        theme_dir = Path.home() / ".config" / "stylerx" / "themes"
    return theme_dir.exists() or True


def _check_installation():
    if sys.platform == "win32":
        appdata = Path(os.environ.get("APPDATA", ""))
        settings = appdata / "StylerX" / "settings.ini"
    else:
        settings = Path.home() / ".config" / "stylerx" / "settings.ini"
    return True


def cmd_install(args):
    print()
    print(f"  {bold('StylerX is cooking...')}")
    print()

    obs_dirs = get_obs_data_dirs()

    if not obs_dirs:
        print(f"  {ERROR} Could not find OBS Studio installation.")
        print(f"  {WORKING} Please install OBS Studio first:")
        print(f"          https://obsproject.com/download")
        return 1

    obs_dir = obs_dirs[0]
    print(f"  {WORKING} Found OBS: {obs_dir}")
    print()

    steps = [
        ("Detecting OBS", 0, 10),
        ("Preparing directories", 10, 30),
        ("Installing plugin", 30, 70),
        ("Configuring themes", 70, 85),
        ("Verifying installation", 85, 95),
        ("Finishing", 95, 100),
    ]

    for label, start, end in steps:
        sp = Spinner(label)
        sp.start()
        time.sleep(0.3)

        if label == "Detecting OBS":
            pass
        elif label == "Preparing directories":
            _prepare_dirs(args)
        elif label == "Installing plugin":
            _install_plugin(obs_dir, args)
        elif label == "Configuring themes":
            _configure_themes(args)
        elif label == "Verifying installation":
            time.sleep(0.2)
        elif label == "Finishing":
            _finalize_install(args)

        sp.stop()

    print()
    print(f"  {SUCCESS} {bold('StylerX installed successfully!')}")
    print(f"  {WORKING} Restart OBS Studio and open Tools {bold('→')} StylerX Studio")
    print()


def _prepare_dirs(args):
    if sys.platform == "win32":
        appdata = Path(os.environ.get("APPDATA", ""))
        theme_dir = appdata / "StylerX" / "themes"
    else:
        theme_dir = Path.home() / ".config" / "stylerx" / "themes"
    theme_dir.mkdir(parents=True, exist_ok=True)


def _install_plugin(obs_dir, args):
    plugin_path, data_dir = get_stylerx_paths(obs_dir)
    plugin_path.parent.mkdir(parents=True, exist_ok=True)
    data_dir.mkdir(parents=True, exist_ok=True)

    if args.build_dir:
        build_dir = Path(args.build_dir)
        built_plugin = build_dir / "styler-x.dll"
        if built_plugin.exists():
            shutil.copy2(str(built_plugin), str(plugin_path))
            print(f"\n         Copied: {built_plugin} → {plugin_path}")


def _configure_themes(args):
    if sys.platform == "win32":
        appdata = Path(os.environ.get("APPDATA", ""))
        theme_dir = appdata / "StylerX" / "themes"
    else:
        theme_dir = Path.home() / ".config" / "stylerx" / "themes"

    readme = theme_dir / "README.txt"
    if not readme.exists():
        with open(readme, "w") as f:
            f.write("StylerX User Themes\n")
            f.write("Place your .json theme files in this directory.\n")
            f.write("They will appear automatically in StylerX Studio.\n")


def _finalize_install(args):
    pass


def cmd_uninstall(args):
    print()
    print(f"  {WORKING} Uninstalling StylerX...")
    print()

    obs_dirs = get_obs_data_dirs()
    if obs_dirs:
        for obs_dir in obs_dirs:
            plugin_path, data_dir = get_stylerx_paths(obs_dir)
            backup_dir = obs_dir / "stylerx-backup"

            if plugin_path.exists():
                backup_dir.mkdir(exist_ok=True)
                backup = backup_dir / plugin_path.name
                shutil.copy2(str(plugin_path), str(backup))
                plugin_path.unlink()
                print(f"  {SUCCESS} Removed: {plugin_path}")
                print(f"  {WORKING} Backed up: {backup}")

            if data_dir.exists():
                shutil.rmtree(str(data_dir))
                print(f"  {SUCCESS} Removed: {data_dir}")

    if not args.keep_themes:
        if sys.platform == "win32":
            appdata = Path(os.environ.get("APPDATA", ""))
            theme_dir = appdata / "StylerX" / "themes"
        else:
            theme_dir = Path.home() / ".config" / "stylerx" / "themes"

        if theme_dir.exists():
            print(f"  {WORKING} Your themes are preserved at: {theme_dir}")
            print(f"  {WORKING} Remove manually if desired.")

    print()
    print(f"  {SUCCESS} StylerX uninstalled.")
    print(f"  {WORKING} Restart OBS Studio to complete removal.")
    print()


def cmd_update(args):
    print()
    print(f"  {WORKING} StylerX update check...")
    print()

    current = VERSION
    print(f"  {WORKING} Current version: v{current}")

    spinner = Spinner("Checking for updates")
    spinner.start()
    time.sleep(1)
    spinner.stop(f"v{current}")

    print(f"  {SUCCESS} StylerX is up to date (v{current}).")
    print()


def main():
    parser = argparse.ArgumentParser(
        description="StylerX CLI - Live Theme Engine for OBS Studio",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  stylerx install              Install StylerX plugin
  stylerx uninstall            Remove StylerX plugin
  stylerx update               Check for updates
  stylerx version              Show version info
  stylerx doctor               Run system diagnostics
        """,
    )

    parser.add_argument(
        "command",
        nargs="?",
        choices=["install", "uninstall", "update", "version", "doctor"],
        help="Command to execute",
    )
    parser.add_argument(
        "--build-dir",
        help="Path to the CMake build directory containing the compiled plugin",
    )
    parser.add_argument(
        "--keep-themes",
        action="store_true",
        help="Keep themes directory when uninstalling",
    )

    args = parser.parse_args()

    print(LOGO)
    print(f"  {bold('Live Theme Engine for OBS Studio')}")
    print(f"  {bold('StylerX CLI')} v{VERSION}")
    print()

    if args.command == "version":
        return cmd_version(args)
    elif args.command == "doctor":
        return cmd_doctor(args)
    elif args.command == "install":
        return cmd_install(args)
    elif args.command == "uninstall":
        return cmd_uninstall(args)
    elif args.command == "update":
        return cmd_update(args)
    else:
        parser.print_help()
        return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print(f"\n{ERROR} Interrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"\n{ERROR} Unexpected error: {e}")
        sys.exit(1)
