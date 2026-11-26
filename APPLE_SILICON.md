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

The `experimental` branch adds optional profiling, AArch64 JIT fixes and a
throttling accuracy fix:

- `TRANSLATION_STATS=true` qmake option reports JIT/interpreter instruction
  counts, translation/write-action/flush counts and the most common untranslated
  ARM opcodes when emulation stops.
- Non-S `UMULL`, `UMLAL`, `SMULL` and `SMLAL` are translated with AArch64
  `UMADDL`/`SMADDL`; S-flag variants still fall back to the interpreter.
  Note that ARM's long-multiply `U` bit is 1 for signed and 0 for unsigned.
- Non-S register-shifted ARM data-processing instructions are translated by
  materializing the barrel-shifter result first; S-flag variants still fall back.
- Cached JIT stores now call `write_action()` when write flags are set, restoring
  write-breakpoint and self-modifying-code handling on AArch64. Invalidating the
  currently executing block is allowed instead of raising an error, because
  cached stores can now trigger that path from inside JIT code.
- CP15 cache/TLB maintenance and the Linux debug-override register no longer
  discard every translation; they refresh the address cache only. MMU control,
  translation-table and domain writes still perform a full flush. The cached
  store hook handles real self-modifying code.
- The flash-access workaround that ignores cache flushes from SRAM code is now
  enabled for all builds, including those with `SUPPORT_LINUX`.
- The macOS throttle wait sleeps to just before its deadline and spins the short
  tail, instead of using `QThread::usleep()` directly. `QThread::usleep()` can
  overshoot by about 1.5 ms per 10 ms interval, which showed up as ~83–87%
  reported speed even when the emulator had spare capacity.

Build the instrumented variant with:

```bash
/opt/homebrew/opt/qt@5/bin/qmake .. TRANSLATION_STATS=true
make -j$(sysctl -n hw.ncpu)
```

## Release 1.7

The `arm64-1.7` release contains the fixes listed above. The DMG is
`firebird-emu-1.7-arm64.dmg` and its SHA-256 is:

```
677a08e2991ab8b69e615406c596aa654a52b2858421499321af0428e61c7db0
```

## Gatekeeper

The released DMG is ad-hoc signed but not notarized, so macOS may block it on
first launch. See the release notes for step-by-step instructions to open it.
