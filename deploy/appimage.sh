#!/bin/bash



print_info() {
    echo -e "\e[1m[INFO ]\e[0m $1" >&2
}

print_warn() {
    echo -e "\e[1;33m[WARN ]\e[0;33m $1\e[0m" >&2
}

print_err() {
    echo -e "\e[1;31m[ERROR]\e[0;31m $1\e[0m" >&2
}

print_err_and_die() {
    print_err "$1"
    exit "${2:-1}"
}

usage() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  -d, --working-dir   Working directory [required]"
    echo "  -c, --clean         Clean working directory after finish"
    echo "      --python        Python executable (must be absolute path)"
    echo "      --venv          Python virtual environment"
    echo "  -h, --help          Display this help message"
    exit 1
}

# https://stackoverflow.com/a/246128
get_script_dir() {
    local SOURCE_PATH="${BASH_SOURCE[0]}"
    local SYMLINK_DIR
    local SCRIPT_DIR
    # Resolve symlinks recursively
    while [ -L "$SOURCE_PATH" ]; do
        # Get symlink directory
        SYMLINK_DIR="$(cd -P "$(dirname "$SOURCE_PATH")" >/dev/null 2>&1 && pwd)"
        # Resolve symlink target (relative or absolute)
        SOURCE_PATH="$(readlink "$SOURCE_PATH")"
        # Check if candidate path is relative or absolute
        if [[ $SOURCE_PATH != /* ]]; then
            # Candidate path is relative, resolve to full path
            SOURCE_PATH=$SYMLINK_DIR/$SOURCE_PATH
        fi
    done
    # Get final script directory path from fully resolved source path
    SCRIPT_DIR="$(cd -P "$(dirname "$SOURCE_PATH")" >/dev/null 2>&1 && pwd)"
    echo "$SCRIPT_DIR"
}

while [[ $# -gt 0 ]]; do
    case "$1" in
    -d | --working-dir)
        WORK_DIR="$2"
        mkdir -p "$WORK_DIR"
        shift 2
        ;;
    --python)
        if [[ "$2" == /* ]]; then
            PYTHON="$2"
            shift 2
        else
            print_err_and_die "Python path is not an absolute path"
        fi
        ;;
    --venv)
        PYTHON_VENV="$(realpath "$2")"
        if [[ -f "$PYTHON_VENV/bin/activate" ]]; then
            print_err_and_die "Python virtual environment missing activation file"
        fi
        if [[ -x "$PYTHON_VENV/bin/activate" ]]; then
            print_err_and_die "Python virtual environment activation file is not executable"
        fi
        shift 2
        ;;
    -h | --help)
        usage
        ;;
    *)
        print_err "Invalid option '$1'." >&2
        usage
        ;;
    esac
done

# Command vars

QT_VER="${QT_VER:-6.12.0}"

SCRIPT_DIR="$(get_script_dir)"

if [[ -z "$WORK_DIR" ]]; then
    WORK_DIR="$SCRIPT_DIR/working-dir"
fi

QT_ROOT="$WORK_DIR/qt/$QT_VER/gcc_64"
SRCS_DIR="$WORK_DIR/src"

# Sources array
declare -A HIDAPI=(
    ["url"]="https://github.com/libusb/hidapi/archive/refs/tags/hidapi-0.15.0.tar.gz"
    ["hash"]="5d84dec684c27b97b921d2f3b73218cb773cf4ea915caee317ac8fc73cef8136"
)

SOURCES=(HIDAPI)

# Prelimery check

if command -v curl >/dev/null 2>&1; then
    print_err_and_die "'curl' is not found"
fi

# Set up Python

if [[ -z "$PYTHON_VENV" ]]; then
    if [[ -z "$PYTHON" ]]; then
        if command -v python >/dev/null 2>&1; then
            PYTHON="$(command -v python)"
        elif command -v python3 >/dev/null 2>&1; then
            PYTHON="$(command -v python3)"
        else
            print_err_and_die "Error: Neither 'python' nor 'python3' was found in PATH."
        fi
    fi

    print_info "Setting up Python virtual environment..."

    PYTHON_VENV="$WORK_DIR/venv"
    "$PYTHON" -m venv "$PYTHON_VENV"
fi

print_info "Activating Python virtual environment..."

source "$PYTHON_VENV/bin/activate"

pip install -U pip

print_info "Installing Qt..."

pip install aqtinstall

aqt install-qt -O "$WORK_DIR/qt" linux desktop "$QT_VER" linux_gcc_64 -m qtimageformats

print_info "Downloading sources..."
