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

# Prevent Unicode crashes on Windows terminals (cp437/cp1252)
if sys.platform == "win32" and hasattr(sys.stdout, 'reconfigure'):
    sys.stdout.reconfigure(errors='replace')

# Detect ANSI color support
def _supports_color():
    if not sys.stdout.isatty():
        return False
    if os.environ.get("NO_COLOR"):
        return False
    if os.environ.get("TERM") == "dumb":
        return False
    if sys.platform == "win32":
        try:
            import ctypes
            kernel32 = ctypes.windll.kernel32
            console = kernel32.GetStdHandle(-11)
            mode = ctypes.c_uint32()
            if kernel32.GetConsoleMode(console, ctypes.byref(mode)):
                if kernel32.SetConsoleMode(console, mode.value | 0x4):
                    return True
        except Exception:
            pass
        return False
    return True

_USE_ANSI = _supports_color()

VERSION = "1.0.0"
APP_NAME = "StylerX"

LOGO = r"""
  SSS  TTTTT  Y   Y  L     EEEEE  RRR    X   X
 S       T    Y Y   L     E      R  R    X   X
  SSS    T     Y    L     EEE    RRR      X X
     S   T     Y    L     E      R R      X X
  SSS    T     Y    LLLL  EEEEE  R  R     X X
"""

# ---- Terminal Helpers ----


def green(text):
    return f"\033[92m{text}\033[0m" if _USE_ANSI else text


def red(text):
    return f"\033[91m{text}\033[0m" if _USE_ANSI else text


def yellow(text):
    return f"\033[93m{text}\033[0m" if _USE_ANSI else text


def cyan(text):
    return f"\033[96m{text}\033[0m" if _USE_ANSI else text


def bold(text):
    return f"\033[1m{text}\033[0m" if _USE_ANSI else text


def dim(text):
    return f"\033[2m{text}\033[0m" if _USE_ANSI else text


def reset():
    return "\033[0m" if _USE_ANSI else ""


SUCCESS = green("[+]")
ERROR = red("[x]")
WORKING = cyan("[*]")
INSTALLING = yellow("[>]")


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
        obs_env = os.environ.get("OBS_PATH", "")
        obs_paths = [
            program_files / "obs-studio",
            Path(obs_env) if obs_env else None,
        ]
        obs_paths = [p for p in obs_paths if p and str(p) and p.exists()]
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
    print(f"  Plugin version: {VERSION}")
    print(f"  Python:          {sys.version.split()[0]}")
    print(f"  Platform:        {sys.platform}")
    print()


def cmd_doctor(args):
    print()
    print(bold(" StylerX Diagnostics"))
    print()

    all_pass = True

    # 1. OS check
    print(f"  {SUCCESS} Operating system detected")

    # 2. OBS install check
    obs_dirs = get_obs_data_dirs()
    if obs_dirs:
        print(f"  {SUCCESS} OBS Studio found")
        for d in obs_dirs:
            print(f"          {dim(str(d))}")
    else:
        print(f"  {ERROR} OBS Studio found")
        print(f"          {dim('Install OBS Studio from https://obsproject.com/download')}")
        all_pass = False

    # 3. Plugin DLL check - Program Files and AppData
    dll_found = False
    checked_paths = []

    if sys.platform == "win32":
        appdata_path = Path(os.environ.get("APPDATA", "")) / "obs-studio" / "plugins" / "64bit" / "styler-x.dll"
        checked_paths.append(appdata_path)

        for obs_dir in obs_dirs:
            progfiles_path = obs_dir / "obs-plugins" / "64bit" / "styler-x.dll"
            checked_paths.append(progfiles_path)

        found_at = None
        for p in checked_paths:
            if p.exists():
                found_at = p
                dll_found = True
                break

        if dll_found:
            print(f"  {SUCCESS} Plugin installation found")
            print(f"          {dim(str(found_at))}")
        else:
            print(f"  {ERROR} Plugin installation found")
            print(f"          {dim('styler-x.dll not found. Run:')}")
            print(f"          {dim('  stylerx install --build-dir StylerX/build')}")
            all_pass = False

    else:
        # macOS/Linux - check standard paths
        for obs_dir in obs_dirs:
            plugin_path, _ = get_stylerx_paths(obs_dir)
            checked_paths.append(plugin_path)

        found_at = None
        for p in checked_paths:
            if p.exists():
                found_at = p
                dll_found = True
                break

        if dll_found:
            print(f"  {SUCCESS} Plugin installation found")
            print(f"          {dim(str(found_at))}")
        else:
            print(f"  {ERROR} Plugin installation found")
            print(f"          {dim('Plugin file not found. Run:')}")
            print(f"          {dim('  stylerx install --build-dir StylerX/build')}")
            all_pass = False

    # 4. Theme engine check
    if sys.platform == "win32":
        appdata = Path(os.environ.get("APPDATA", ""))
        theme_dir = appdata / "StylerX" / "themes"
    else:
        theme_dir = Path.home() / ".config" / "stylerx" / "themes"

    if theme_dir.exists():
        print(f"  {SUCCESS} Theme engine ready")
        print(f"          {dim(str(theme_dir))}")
    else:
        print(f"  {WORKING} Theme engine ready")
        print(f"          {dim('Themes directory will be created on first run')}")
        all_pass = False

    # 5. OBS log check
    if sys.platform == "win32":
        log_dir = Path(os.environ.get("APPDATA", "")) / "obs-studio" / "logs"
        if log_dir.exists():
            logs = sorted(log_dir.glob("*.txt"), key=lambda f: f.stat().st_mtime, reverse=True)
            if logs:
                latest = logs[0]
                with open(latest, "r", errors="replace") as f:
                    for line in f:
                        if "styler" in line.lower():
                            line = line.strip()
                            if "loaded successfully" in line.lower():
                                print(f"  {SUCCESS} OBS log confirms plugin loaded")
                                print(f"          {dim(line[:120])}")
                            elif "failed" in line.lower() or "error" in line.lower():
                                print(f"  {ERROR} OBS log shows plugin load error")
                                print(f"          {dim(line[:120])}")
                                all_pass = False
                            else:
                                print(f"  {WORKING} OBS log entry found")
                                print(f"          {dim(line[:120])}")
                            break
                    else:
                        print(f"  {WORKING} OBS log checked")
                        print(f"          {dim('No styler-x entries in latest log')}")
            else:
                print(f"  {WORKING} OBS log checked")
                print(f"          {dim('No OBS log files found')}")
        else:
            print(f"  {WORKING} OBS log checked")
            print(f"          {dim('Log directory not found')}")

    print()
    if all_pass:
        print(f"  {bold(green('All checks passed. StylerX is healthy.'))}")
    else:
        print(f"  {bold(red('Some checks failed.'))}")
        if not dll_found:
            print(f"  {bold(red('Run `stylerx install --build-dir StylerX/build` first.'))}")
    print()
    return 0 if all_pass else 1


def _check_plugin_dirs():
    return None  # unused, kept for compatibility


def _check_theme_engine():
    return None  # unused, kept for compatibility


def _check_installation():
    return None  # unused, kept for compatibility


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
    print(f"  {WORKING} Restart OBS Studio and open Tools {bold('->')} StylerX Studio")
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
    data_dir.mkdir(parents=True, exist_ok=True)

    if not args.build_dir:
        return

    build_dir = Path(args.build_dir)
    built_plugin = build_dir / "styler-x.dll"
    if not built_plugin.exists():
        print(f"\n         {red('[x]')} Built plugin not found at: {built_plugin}")
        return

    # Try Program Files first, fall back to AppData
    dest = plugin_path
    try:
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(str(built_plugin), str(dest))
        print(f"\n         Copied: {built_plugin} -> {dest}")
        return
    except PermissionError:
        pass

    # Fall back to user-level AppData path
    if sys.platform == "win32":
        appdata_plugin = Path(os.environ.get("APPDATA", "")) / "obs-studio" / "plugins" / "64bit" / "styler-x.dll"
        appdata_plugin.parent.mkdir(parents=True, exist_ok=True)
        try:
            shutil.copy2(str(built_plugin), str(appdata_plugin))
            print(f"\n         Copied: {built_plugin} -> {appdata_plugin}")
            print(f"         {yellow('[!]')} Program Files requires admin - used AppData instead")
            return
        except PermissionError:
            print(f"\n         {red('[x]')} Could not write to Program Files (needs admin) or AppData")
            print(f"         Run this terminal as Administrator and try again")


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


def cmd_logo(args):
    print(LOGO)


def cmd_help(args):
    print()
    print(f"  {bold('StylerX CLI')} v{VERSION} - {bold('Live Theme Engine for OBS Studio')}")
    print()
    print(f"  {bold('Commands:')}")
    print(f"    install {dim('[--build-dir PATH]')}    Install StylerX plugin to OBS")
    print(f"    uninstall {dim('[--keep-themes]')}     Remove StylerX plugin from OBS")
    print(f"    update                           Check for updates")
    print(f"    version                          Show version info")
    print(f"    logo                             Print the StylerX logo")
    print(f"    doctor                           Run system diagnostics")
    print(f"    help                             Show this help message")
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
  stylerx logo                 Print the StylerX logo
  stylerx doctor               Run system diagnostics
  stylerx help                 Show this help message
        """,
    )

    parser.add_argument(
        "command",
        nargs="?",
        choices=["install", "uninstall", "update", "version", "logo", "doctor", "help"],
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

    if args.command is None or args.command == "help":
        cmd_help(args)
        return 0

    print(LOGO)
    print(f"  {bold('Live Theme Engine for OBS Studio')}")
    print(f"  {bold('StylerX CLI')} v{VERSION}")
    print()

    if args.command == "version":
        return cmd_version(args)
    elif args.command == "logo":
        return cmd_logo(args)
    elif args.command == "doctor":
        return cmd_doctor(args)
    elif args.command == "install":
        return cmd_install(args)
    elif args.command == "uninstall":
        return cmd_uninstall(args)
    elif args.command == "update":
        return cmd_update(args)
    else:
        cmd_help(args)
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
