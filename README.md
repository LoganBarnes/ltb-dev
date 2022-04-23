# ltb-dev

A top level development repository containing all the `ltb-*` projects.

## Checkout

### Https

```bash
git clone --recurse-submodules -j8 https://github.com/LoganBarnes/ltb-dev.git
cd ltb-fun
```

or

```bash
git clone https://github.com/LoganBarnes/ltb-dev.git
cd ltb-fun
git submodule update --init --recursive
```

### SSH

```bash
git clone --recurse-submodules -j8 git@github.com:LoganBarnes/ltb-dev.git
cd ltb-fun
```

or

```bash
git clone git@github.com:LoganBarnes/ltb-dev.git
cd ltb-fun
git submodule update --init --recursive
```

## Configure

### Unix

```bash
# From project root dir
mkdir build
cd build
cmake -G"Ninja Multi-Config" -DCMAKE_BUILD_TYPE=Debug ..
```

### Windows

```powershell
# From project root dir
mkdir build
cd build
cmake -G"Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Debug ..
```

## Build

To build all the targets:

```bash
# From build dir
cmake --build . --config Release --parallel # Release build
cmake --build . --config Debug --parallel # Debug build
```

or to build a specific target:

```bash
# From build dir
cmake --build . --config Release --target <target> --parallel # Release build
cmake --build . --config Debug --target <target> --parallel # Debug build
```

## Run

### Tests

```bash
# From build dir
ctest -C Release # Test Release build
ctest -C Debug # Test Debug build
```

### Executables

```bash
./_deps/ltb<lib>-build/Release/<executable> # Release executable
./_deps/ltb<lib>-build/Debug/<executable> # Debug executable
```

### Documentation

[pipx](https://pypa.github.io/pipx/) for managing python dependencies (can also just use `pip`).

```bash
python3 -m pip install --user pipx
python3 -m pipx ensurepath
```

[Sphinx](https://www.sphinx-doc.org/en/master/) for building web pages from the documentation.

```bash
# The main Markdown to HTML generation tool
pipx install sphinx
# Add extensions and themes to sphinx
pipx inject sphinx breathe myst-parser sphinxcontrib-mermaid furo exhale
# Add a tool to enable hot reloading when editing docs
pipx inject sphinx sphinx-autobuild --include-apps
```
