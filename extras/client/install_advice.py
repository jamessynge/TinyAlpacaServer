"""Provide advice on how to 'pip install' a package."""

import os
import sys


def install_advice(package_name: str, do_exit=True, version_constraint=''):
  """Provide advice on how to 'pip install' a package."""
  if os.name == 'nt':
    cmd = 'py'
  else:
    cmd = 'python3'
  print(
      f"""
Unable to find Python module "{package_name}".
Try these commands to install it using pip:

# Ensure pip is installed...
{cmd} -m ensurepip --default-pip

# Ensure pip and related tools are up to date...
{cmd} -m pip install --upgrade pip setuptools wheel

# Install package "{package_name}"
{cmd} -m pip install "{package_name}{version_constraint}"
""",
      flush=True,
      file=sys.stderr)
  if do_exit:
    sys.exit(1)
