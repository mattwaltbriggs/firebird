# Apple Silicon build notes

This fork provides a native `arm64` build of Firebird Emu for Apple Silicon Macs.
The released build is upstream source unchanged apart from this document; the
upstream project already contains an AArch64 JIT backend.

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

## Experimental JIT branch

The `experimental` branch adds optional profiling and three AArch64 JIT fixes:

- `TRANSLATION_STATS=true` qmake option reports JIT/interpreter instruction
  counts and the most common untranslated ARM opcodes when emulation stops.
- Non-S `UMULL`, `UMLAL`, `SMULL` and `SMLAL` are translated with AArch64
  `UMADDL`/`SMADDL`; S-flag variants still fall back to the interpreter.
- Non-S register-shifted ARM data-processing instructions are translated by
  materializing the barrel-shifter result first; S-flag variants still fall back.
- Cached JIT stores now call `write_action()` when write flags are set, restoring
  write-breakpoint and self-modifying-code handling on AArch64.

Build the instrumented variant with:

```bash
/opt/homebrew/opt/qt@5/bin/qmake .. TRANSLATION_STATS=true
make -j$(sysctl -n hw.ncpu)
```

## Gatekeeper

The released DMG is ad-hoc signed but not notarized, so macOS may block it on
first launch. See the release notes for step-by-step instructions to open it.
