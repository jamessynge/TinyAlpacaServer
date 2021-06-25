#!/usr/bin/env python3
"""Update literals.inc with literals defined in other files.

And determine which literals are actually in use, flag for removal.
"""

from typing import Sequence, Dict

from absl import app


def process_source_file(fpath: str) -> Dict[str, str]:
  # Read source file as a single string, repeatedly use regexp to locate start
  # of a DEFINE_LITERAL, then other regexp to pull out the name and value of
  # the literal.
  return {fpath: fpath}  # Just to shut up the linter until I write the impl.


def main(argv: Sequence[str]) -> None:
  if len(argv) > 1:
    raise app.UsageError('Too many command-line arguments.')


if __name__ == '__main__':
  app.run(main)
