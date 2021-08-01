#!/bin/bash -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

if [[ "$(basename "${SCRIPT_DIR}")" != "dev_tools" ]]
then
  echo "The script is not in the expected directory"
  exit 1
fi

cd "${SCRIPT_DIR}"
cd ../../src

if [[ ! -f TinyAlpacaServer.h ]]
then
  echo "Expected to find TinyAlpacaServer.h in the src directory."
  exit 1
fi

G4_PATH="$(which g4)"

if [[ -n "${G4_PATH}" && -x "${G4_PATH}" ]]
then
  cd "$(g4 g4d)/"
  SEARCH_PAT="experimental/users/jamessynge/arduino/tas/src"
else
  SEARCH_PAT="*"
fi

for P in $(find $SEARCH_PAT -type f -name '*.h' | egrep -v "[A-Z]" | sort)
do
  echo "#include \"$P\"  // IWYU pragma: export"
done
