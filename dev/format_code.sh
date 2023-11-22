#!/usr/bin/env bash
set -euo pipefail

# Formats files according our coding style.
#
# Usage:
#
#   format_code.sh [--all] [--sudo] [--docker]
#
# Notes:
#   - Run the command "./dev/format_code.sh" in the root of the repository.
#   - If you have Docker installed (on Linux), it's recommended to pass the
#     "--docker" parameter. A docker image containing all required tools will
#     then be created and used so you don't have to install any dependencies.
#   - To run docker with sudo, use the "--sudo" parameter.
#   - Without docker, make sure the executables "git", "clang-format",
#     "cmake-format", "python3" and "xmlsort" are available in PATH.
#   - To format all files (instead of only modified ones), add the "--all"
#     parameter. This is intended only for LibrePCB maintainers, usually you
#     should not use this!

DOCKER=""
DOCKER_CMD="docker"
CLANGFORMAT=${CLANGFORMAT:-clang-format}
ALL=""
for i in "$@"
do
case $i in
  --docker)
  DOCKER="--docker"
  shift
  ;;
  --sudo)
  DOCKER_CMD="sudo docker"
  shift
  ;;
  --all)
  ALL="--all"
  shift
  ;;
esac
done

REPO_ROOT=$(git rev-parse --show-toplevel)

if [ "$DOCKER" == "--docker" ]; then
  DOCKER_IMAGE=librepcb/devtools:1.0.0

  if [ "$($DOCKER_CMD images -q $DOCKER_IMAGE | wc -l)" == "0" ]; then
    echo "Building devtools container..."
    $DOCKER_CMD build "$REPO_ROOT/dev/devtools" -t librepcb/devtools:1.0.0
  fi

  echo "[Re-running format_code.sh inside Docker container]"
  $DOCKER_CMD run --rm -t --user "$(id -u):$(id -g)" \
    -v "$REPO_ROOT:/code" \
    $DOCKER_IMAGE \
    /usr/bin/env bash -c "cd /code && dev/format_code.sh $ALL"

  echo "[Docker done.]"
  exit 0
fi

COUNTER=0

# this function searches for all files that are processable in directory $1 with
# extensions with mask that follows. It searchs sorted for tracked files first,
# then following untracked files
#
# All found files are put on stdout so it is parsable by foreach loop
search_files() {
  DIRECTORY=$1
  shift

  MASKS=""
  while (($#)); do
  	MASKS+=" ${DIRECTORY}**$1"
    shift
  done

  if [ "$ALL" == "--all" ]; then
    TRACKED=$(git ls-files -- $MASKS)
  else
    # Only files which differ from the master branch
    TRACKED=$(git diff --name-only master -- $MASKS)
  fi
  UNTRACKED=$(git ls-files --others --exclude-standard -- $MASKS)

  for file in $TRACKED $UNTRACKED
  do
    if [ -f "$file" ]; then
      echo "$file"
    fi
  done
}

# this function tracks modifications of file and prints out information that file
# has been processed by the script. It increment processed file counter.
# The processed file contents is in the GLOBAL variable NEW_CONTENT
update_file() {
# Note: Do NOT use in-place edition of the tools because these causes
# "make" to detect the files as changed every time, even if the content was
# not modified! So we only overwrite the files here if their content has changed.

  OLD_CONTENT=$(cat "$1")

  if [ "$NEW_CONTENT" != "$OLD_CONTENT" ]
  then
    printf "%s\n" "$NEW_CONTENT" > "$1"
    echo "[M] $1"
    COUNTER=$((COUNTER+1))
  else
    echo "[ ] $1"
  fi
}

# Format source files with clang-format and Python 3.
clang_format_failed() {
  echo "" >&2
  echo "ERROR: clang-format failed!" >&2
  echo "  Make sure that clang-format 6 and Python 3 are installed." >&2
  echo "  On Linux, you can also run this script in a docker" >&2
  echo "  container by using the '--docker' argument." >&2
  exit 7
}
echo "Formatting sources with $CLANGFORMAT and Python..."
for dir in apps/ libs/librepcb/ tests/unittests/
do
  for file in $(search_files "${dir}" .cpp .hpp .h)
  do
    NEW_CONTENT=$($CLANGFORMAT -style=file "$file" || clang_format_failed)
    NEW_CONTENT=$(echo "$NEW_CONTENT" | "$REPO_ROOT/dev/format_code_helper.py" "$file" || clang_format_failed)
    update_file "$file"
  done
done

# Format *.ui files with Python 3.
ui_format_failed() {
  echo "" >&2
  echo "ERROR: Python failed!" >&2
  echo "  Make sure that Python 3 is installed." >&2
  echo "  On Linux, you can also run this script in a docker" >&2
  echo "  container by using the '--docker' argument." >&2
  exit 7
}
echo "Formatting UI files with Python..."
for dir in apps/ libs/librepcb/ tests/unittests/
do
  for file in $(search_files "${dir}" .ui)
  do
    NEW_CONTENT=$(cat "$file" | "$REPO_ROOT/dev/format_code_helper.py" "$file" || ui_format_failed)
    update_file "$file"
  done
done

# Format CMake files with cmake-format.
cmake_format_failed() {
  echo "" >&2
  echo "ERROR: cmake-format failed!" >&2
  echo "  Make sure that cmake-format is installed." >&2
  echo "  On Linux, you can also run this script in a docker" >&2
  echo "  container by using the '--docker' argument." >&2
  exit 7
}
echo "Formatting CMake files with cmake-format..."
for file in $(search_files "" CMakeLists.txt .cmake)
do
  NEW_CONTENT=$(cmake-format "$file" || cmake_format_failed)
  update_file "$file"
done

# Format *.qrc files with xmlsort.
xmlsort_failed() {
  echo "" >&2
  echo "ERROR: xmlsort failed!" >&2
  echo "  Make sure that xmlsort is installed." >&2
  echo "  On Linux, you can also run this script in a docker" >&2
  echo "  container by using the '--docker' argument." >&2
  exit 7
}
echo "Formatting resource files with xmlsort..."
for dir in img/ libs/
do
  for file in $(search_files "${dir}" .qrc)
  do
    NEW_CONTENT=$(xmlsort -r "RCC/qresource/file" -i -s "$file" || xmlsort_failed)
    NEW_CONTENT="${NEW_CONTENT//\'/\"}"
    update_file "$file"
  done
done

# Format qml source files with qmlformat
qml_format_failed() {
  echo "" >&2
  echo "ERROR: qmlformat failed!" >&2
  echo "  Make sure that qmlformat (qtdeclarative5-dev-tools) are installed." >&2
  echo "  On Linux, you can also run this script in a docker" >&2
  echo "  container by using the '--docker' argument." >&2
  exit 7
}
echo "Formatting QML sources with qmlformat..."
for dir in share/
do
  for file in $(search_files "${dir}" .qml)
  do
    if [ -f "$file" ]; then
      NEW_CONTENT=$(/usr/lib/qt5/bin/qmlformat "$file" || qml_format_failed)
      update_file "$file"
    fi
  done
done

echo "Finished: $COUNTER files modified."
