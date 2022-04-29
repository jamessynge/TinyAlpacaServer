#!/usr/bin/env python3
"""Tooling for reading and updating files."""

import sys
from typing import Optional, Sequence, Tuple


class FileContents:
  """Path to a file and its contents."""

  def __init__(self, file_path: str = '', raw_source: str = ''):
    self.file_path = file_path
    if not raw_source:
      with open(file_path, mode='rt') as f:
        raw_source: str = f.read()
    self.raw_source: str = raw_source
    self.original_raw_source: str = raw_source

  def update_file(self):
    with open(self.file_path, mode='wt') as f:
      f.write(self.raw_source)

  def replace_section(self, section_contents: str, start_pos: int,
                      end_pos: int) -> None:
    """Replace the text between two positions."""
    if not (0 <= start_pos and start_pos <= end_pos and
            end_pos <= len(self.raw_source)):
      raise IndexError(
          f'Section location [{start_pos}, {end_pos}] is not in [0, {len(self.raw_source)}]'
      )
    self.raw_source = ''.join([
        self.raw_source[0:start_pos], section_contents,
        self.raw_source[end_pos:]
    ])

  def find_tagged_section(
      self,
      start_tag: str,
      end_tag: str,
      section_includes_tags: bool = False) -> Optional[Tuple[int, int]]:
    """Find a section of text between a start_tag and an end_tag.

    This supports having a file that contains both generated and manually edited
    text, with the generated text being surrounded by strings that mark the
    start and end of that generated text. For example:

        // Begin Generated Text - DO NOT EDIT
        text that
        was generated
        // End Generated Text

    Note that there is no checking for the presence of multiple start or end
    tags.

    Args:
      start_tag: String to search for in raw_source, representing the start of
        the tagged section. If empty, then the start position is the start of
        the file. One of start_tag and end_tag may be empty, but not both.
      end_tag: String to search for in raw_source, representing the end of the
        tagged section. If empty, then the end position is the end of the file.
      section_includes_tags: If True, the range of text denoted by the returned
        positions include the tags; otherwise the range of text starts
        immediately after a start_tag, and ends with the start of the next
        end_tag.

    Returns:
    """
    if not start_tag and not end_tag:
      raise ValueError('Must specify at least one tag')

    if start_tag:
      start_pos = self.raw_source.find(start_tag)
      if start_pos < 0:
        return None
      search_pos = start_pos + len(start_tag)
      if not section_includes_tags:
        start_pos = search_pos
    else:
      start_pos = 0
      search_pos = 0

    if end_tag:
      end_pos = self.raw_source.find(end_tag, search_pos)
      if end_pos < 0:
        return None
      if section_includes_tags:
        end_pos += len(end_tag)
    else:
      end_pos = len(self.raw_source)

    return (start_pos, end_pos)

  def replace_tagged_section(self,
                             start_tag: str,
                             end_tag: str,
                             replacement: str,
                             retain_tags: bool = True) -> bool:
    """Replaced existing tagged section with new contents.

    To remove a tagged section entirely, pass and empty string for replacement
    and False for retain_tags.

    Args:
      start_tag: String to search for in raw_source, representing the start of
        the tagged section. If empty, then the start position is the start of
        the file. One of start_tag and end_tag may be empty, but not both.
      end_tag: String to search for in raw_source, representing the end of the
        tagged section. If empty, then the end position is the end of the file.
      replacement: String to insert in raw_source in place of the existing
        tagged section.
      retain_tags: If True, replaces the contents between the two existing tags;
        if False, replaces the contents from the beginning of the start_tag
        through to the end of end_tag in raw_source.

    Returns:
      True if the tags were found in the file and the contents were replaced.
    """

    section_positions = self.find_tagged_section(
        start_tag, end_tag, section_includes_tags=not retain_tags)
    if not section_positions:
      return False
    self.replace_section(replacement, *section_positions)
    return True


def process_file(file_path: str):
  """TBD."""
  print()
  print('#' * 80)
  print('Processing file', file_path, flush=True)


def main(argv: Sequence[str]) -> None:
  for arg in argv[1:]:
    process_file(arg)


if __name__ == '__main__':
  main(sys.argv)
  sys.stdout.flush()
