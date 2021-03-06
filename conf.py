# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import textwrap

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))

# -- Project information -----------------------------------------------------

project = 'LTB Development'
copyright = '2022, Logan Barnes'
author = 'Logan Barnes'

# The full version, including alpha/beta/rc tags
release = '1.0.0'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    'sphinx_rtd_theme',
    # Doxygen interop
    'breathe',
    'exhale',
    # Pretty graphs
    'sphinxcontrib.mermaid',
    # Markdown support
    'myst_parser',
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['docs/_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

html_favicon = 'docs/_static/favicon.ico'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['docs/_static']

# These paths are either relative to html_static_path
# or fully qualified paths (eg. https://...)
html_css_files = [
]

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}

# -- Extension Configuration -------------------------------------------------

myst_enable_extensions = [
]

# Set up the breathe extension
breathe_default_project = "ltb"

breathe_projects = {
    "ltb": "docs/_doxygen/xml"
}

# Setup the exhale extension
exhale_args = {
    # These arguments are required
    "containmentFolder": "docs/_api",
    "rootFileName": "library_root.rst",
    "doxygenStripFromPath": ".",
    # Heavily encouraged optional argument (see docs)
    "rootFileTitle": "APIs",
    # Optional arguments
    "createTreeView": True,
    "exhaleExecutesDoxygen": True,
    "exhaleDoxygenStdin": textwrap.dedent('''
        INPUT              = .
        FILE_PATTERNS      = **/include/ltb/**/*.hpp **/include/ltb/**/*.cuh
        EXTENSION_MAPPING += cuh=c++        
    '''),
    "contentsDirectives": True,
    "lexerMapping": {
        r".*\.cuh": "cuda",
    },
}

# Tell sphinx what the primary language being documented is.
primary_domain = 'cpp'

# Tell sphinx what the pygments highlight language should be.
highlight_language = 'cpp'
