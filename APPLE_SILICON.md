# Apple Silicon build notes

This fork provides a native `arm64` build of Firebird Emu for Apple Silicon Macs.
The upstream project already contains an AArch64 JIT backend, so no emulator source
changes were required.

## Build

Upstream CI uses Qt 5.15. The default `qmake` on many Homebrew setups is Qt 6,
which does not compile this version of Firebird unchanged
(`qRegisterMetaTypeStreamOperators` was removed in Qt 6). Use Qt 5 explicitly:

```bash
git clone https://github.com/mattwaltbriggs/firebird.git
cd firebird
git submodule update --init --recursive
mkdir build && cd build
/opt/homebrew/opt/qt@5/bin/qmake ..
make -j$(sysctl -n hw.ncpu)
/opt/homebrew/opt/qt@5/bin/macdeployqt firebird-emu.app -qmldir=../qml
open firebird-emu.app
```

`macdeployqt` with Homebrew Qt 5 misses two transitive dylibs,
`libbrotlicommon.1.dylib` and `libsharpyuv.0.dylib`. Copy them from
`/opt/homebrew/opt/brotli/lib` and `/opt/homebrew/opt/webp/lib` into
`firebird-emu.app/Contents/Frameworks` and adjust their install names with
`install_name_tool` if you bundle the app yourself.

## Gatekeeper

The released DMG is ad-hoc signed but not notarized, so macOS may block it on
first launch. See the release notes for step-by-step instructions to open it.
