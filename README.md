# DALS: Delay-Driven Approximate Logic Synthesis

## Prerequisites
### macOS

The `gcc` alias on macOS is actually `clang`. For compatibility, GNU `gcc`, instead of `clang` provided by macOS, is taken into use.

First, install `gcc` using `homebrew`.

```bash
brew install gcc@8
```

Install `boost` via `homebrew`. Make sure that `--cc=gcc-8` flag is added for the installation command. 
Otherwise `clang` compiled version will be installed and mistakes will happen during linking.

```bash
brew install boost --cc=gcc-8
```

To prevent `boost` from being updated/upgraded to `clang` compiled version, it needs to be pinned.

```bash
brew pin boost
```

### Ubuntu 18.04

Install the latest version of `boost` library.
```bash
sudo apt install libboost-all-dev
```

## Project Structure

- include: header files
- src: source codes
- abc: Berkeley ABC
- abc-plus: C++ wrapper of Berkeley ABC
- benchmark: benchmarks
- out: output approximate circuits
