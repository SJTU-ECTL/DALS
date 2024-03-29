# DALS: Delay-Driven Approximate Logic Synthesis

Refactored version of the original DALS: Delay-Driven Approximate Logic Synthesis, using personal thin wrapper abc-plus.

## Reference
Zhuangzhuang Zhou, Yue Yao, Shuyang Huang, Sanbao Su, Chang Meng, and Weikang Qian, "[DALS: Delay-driven approximate logic synthesis](https://umji.sjtu.edu.cn/~wkqian/papers/Zhou_Yao_Huang_Su_Meng_Qian_DALS_Delay_Driven_Approximate_Logic_Synthesis.pdf)," in *Proceedings of the 2018 IEEE/ACM International Conference on Computer-Aided Design (ICCAD)*, San Diego, CA, USA, 2018, pp. 86:1-86:7.


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
