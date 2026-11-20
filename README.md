# Secret-Santa

A small app for Windows, macOS and Linux that draws Secret Santa names fairly and saves a private "envelope" file for each person. The result is never shown on screen.

## Download

Get the file for your system from the [latest release](https://github.com/XG-PRO/Secret-Santa/releases/latest):

- **Windows:** `Secret-Santa-Windows.exe`. Run it.
- **macOS:** `Secret-Santa-macOS.zip`. Unzip it and open *Secret Santa*. The app isn't signed by Apple, so the first time macOS blocks it: go to System Settings → Privacy & Security and click *Open Anyway*.
- **Linux:** `Secret-Santa-Linux.tar.gz`. Extract it and run `./Secret\ Santa`. It needs X11 (or XWayland), which desktop distros include.

## Use

1. Type each name and press Enter.
2. Leave **No two people draw each other** ticked if you don't want A to buy for B while B buys for A.
3. Choose where to save the envelopes and click **Draw names**.

Each person gets a text file naming who they buy for: `<Name>/<Name>.txt`, or just `<Name>.txt` if you untick **Put each file in its own folder**. Send everyone only their own file.

## How it works

The names are shuffled with the operating system's secure random generator. If anyone drew themselves, or two people drew each other when that rule is on, it shuffles again. Every valid draw is equally likely.

## Build

You need CMake 3.20+, a C++20 compiler and git. On Linux you also need the X11 headers (see [the workflow](.github/workflows/build.yml)).

```
cmake -S . -B build
cmake --build build --config Release
```

The [FLTK](https://www.fltk.org) GUI library is downloaded and linked statically during the build. Pushing a `v*` tag builds all three platforms on GitHub Actions and publishes a release.
