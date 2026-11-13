#!/usr/bin/env python3

from argparse import ArgumentParser
from typing import Protocol
from pathlib import Path
import logging
import shutil
import subprocess
import sys
import os

PIP_OK = False

ARGS: CliArguments = None

LOG = logging.getLogger("hk-appimage-deploy")


class CliArguments(Protocol):
    working_dir: Path
    clean_up: bool
    qt_version: str
    redownload_qt: bool


class LoggingFormatter(logging.Formatter):
    _grey = "\x1b[38;20m"
    _yellow = "\x1b[33;20m"
    _red = "\x1b[31;20m"
    _bold_red = "\x1b[31;1m"
    _reset = "\x1b[0m"

    _format = "%(asctime)s [%(levelname)s] %(message)s"

    _FORMATS = {
        logging.DEBUG: _grey + _format + _reset,
        logging.INFO: _format,
        logging.WARNING: _yellow + _format + _reset,
        logging.ERROR: _red + _format + _reset,
        logging.CRITICAL: _format + _format + _reset,
    }

    def format(self, record):
        log_fmt = self._FORMATS.get(record.levelno)
        return logging.Formatter(log_fmt).format(record)


def check_pip():
    global PIP_OK

    if PIP_OK:
        return

    try:
        # Runs 'python -m pip --version' using the current Python executable
        subprocess.run([sys.executable, '-m', 'pip', '--version'],
                       stdout=subprocess.DEVNULL,
                       stderr=subprocess.DEVNULL)
    except Exception:
        LOG.fatal("Pip is not installed!")

    PIP_OK = True


def python_run(args: [str]):
    subprocess.run([sys.executable] + args, check=True)


def python_venv_run(args: [str]):
    venv = ARGS.working_dir / "venv"
    if venv.exists():
        if not venv.is_dir():
            venv.unlink()
    else:
        subprocess.run([sys.executable, "-m", "venv", str(venv)], check=True)

    subprocess.run([str(venv / "bin" / "python")] + args, check=True)


def download_qt():
    qt_root = ARGS.working_dir / "qt"

    qt_version_root = qt_root / ARGS.qt_version / "gcc_64"

    if qt_version_root.exists() and not ARGS.redownload_qt:
        LOG.info("Skipping downloading Qt...")
        return

    LOG.info("Downloading Qt...")

    aqt_path = shutil.which("aqt")

    run_args = [
        "install-qt", "-O",
        str(ARGS.working_dir / "qt"), "linux", "desktop", ARGS.qt_version,
        "linux_gcc_64", "-m", "qtimageformats"
    ]

    if not aqt_path:
        try:
            subprocess.run([sys.executable, '-m', "aqt", "--help"],
                           stdout=subprocess.DEVNULL,
                           stderr=subprocess.DEVNULL)

            LOG.info("Using system aqt module")

            python_run(['-m', "aqt"] + run_args)
        except Exception:
            LOG.info("Installing aqt...")

            python_venv_run(["-m", "pip", "install", "aqtinstall"])
            python_venv_run(["-m", "aqt"] + run_args)
    else:
        LOG.info(f"Using system aqt: {aqt_path}")

        subprocess.run([aqt_path] + run_args, check=True)


def parse_args():
    parser = ArgumentParser(
        description="HeadsetKontrol AppImage deploy script")

    parser.add_argument("-d",
                        "--working-dir",
                        type=Path,
                        default=(Path(__file__).resolve().parent /
                                 "working-dir"),
                        help="The working directory")

    parser.add_argument("-c",
                        "--clean-up",
                        action='store_true',
                        help="Clean working directory after finish")

    parser.add_argument("--qt-version", type=str, help="Qt version")

    parser.add_argument("--redownload-qt",
                        action='store_true',
                        help="Redownload Qt")

    global ARGS
    ARGS = parser.parse_args()  # type: ignore[assignment]

    if not ARGS.qt_version:
        ARGS.qt_version = os.getenv("QT_VERSION", "6.12.0")


if __name__ == "__main__":
    handler = logging.StreamHandler()
    handler.setFormatter(LoggingFormatter())
    LOG.addHandler(handler)
    LOG.setLevel(logging.DEBUG if os.getenv("DEPLOY_DEBUG") else logging.INFO)

    parse_args()

    # Prelimary check

    download_qt()
