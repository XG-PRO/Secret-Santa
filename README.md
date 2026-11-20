# Secret-Santa

A small Windows app that draws Secret Santa names fairly and saves a private "envelope" file for each person. The result is never shown on screen.

## Use

1. Download `Secret.Santa.exe` from the [latest release](https://github.com/XG-PRO/Secret-Santa/releases/latest) and run it.
2. Type each name and press Enter. Select a name and press Delete to remove it.
3. Leave **No two people draw each other** ticked if you don't want A to buy for B while B buys for A.
4. Choose a folder and click **Draw names**.

Each person gets a folder, `<Name>\<Name>.txt`, naming who they buy for. Send everyone only their own file.

## How it works

The names are shuffled with the operating system's secure random generator (Fisher-Yates). If anyone drew themselves, or two people drew each other when that rule is on, it shuffles again. Every valid draw is equally likely.

## Build

Needs g++ for Windows, e.g. `winget install BrechtSanders.WinLibs.POSIX.UCRT`. Run `src\build.bat`, which creates `Secret Santa.exe` in the repository folder.
