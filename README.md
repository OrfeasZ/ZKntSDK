# ZKnt Mod SDK

The name's Bloke, James Bloke. A modding SDK and mod loader for 007 First Light.

## Description

This is a modding SDK and mod loader for 007 First Light (Project KNT). Its purpose is to allow users to easily install
and use mods, and make the creation of complex runtime mods easier for developers.

## Usage (mod loader & mods)

1. Download the latest version of the mod loader and the mods by
   going [here](https://github.com/OrfeasZ/ZKntSdk/releases), and downloading `ZKntSdk-Release.zip`. This zip file
   contains the mod loader and a few sample mods. Nightly builds can be downloaded
   from [here](https://github.com/OrfeasZ/ZKntSDK/actions) but might be less stable or break in exciting ways!

2. Extract the contents of the `ZKntSdk-Release.zip` archive to your 007 First Light installation directory.

3. Run the game like you normally would.

4. When the game opens, press the key under `ESC` (`~` or `^` on QWERTZ layouts) to open the SDK menu. From there you
   can enable and configure the mods you'd like to use.

5. ...

6. Profit?

> NOTE: Some mods might require additional setup. For mods bundled with the SDK, refer to
> the [table below](#sample-mods).

## Sample mods

There are a few sample mods included in this repository that can be used either as reference or for regular gameplay.

| Mod name                | Description                                                                                            |
|-------------------------|--------------------------------------------------------------------------------------------------------|
| [FreeCam](Mods/FreeCam) | Adds support for an in-game free camera. The free camera can be toggled by pressing `K`.               |
| [NoPause](Mods/NoPause) | Prevents the game from automatically pausing after losing focus (e.g. when alt-tabbing or minimizing). |
| [Noclip](Mods/Noclip)   | Adds a noclip mode that allows free player movement through the world without collision.               |

## Uninstalling

To de-activate / uninstall the mod loader, simply delete `dinput8.dll` from your `007 First Light\Retail` directory. You
can also delete all the other files you previously extracted.

### Building

The SDK requires a recent version of MSVC tools, CMake, and Ninja (or Visual Studio with those workloads installed). On
Linux, cross-compilation to Windows is supported via `clang-cl` and `xwin`, however it's only been tested on NixOS with
CLion.

If you want the SDK and its mods to be automatically installed to the game's folder during the CMake install action,
copy `CMakeUserPresets.json-sample_win` or `CMakeUserPresets.json-sample_linux` (based on your current OS) to
`CMakeUserPresets.json` and change the `GAME_INSTALL_PATH` to point to your game's installation directory.

#### Windows

1. Clone the repo.
2. Initialize and update submodules (`git submodules update --init --recursive`).
3. Open the project in Visual Studio or CLion (for CLion you'll have to manually enable the relevant presets).
4. Build.

#### Linux

1. Clone the repo.
2. Initialize and update submodules (`git submodules update --init --recursive`).
3. Get into a shell with the required toolchain, fetch xwin, and build, as seen below.

```bash
nix develop
./cmake/scripts/fetch-xwin.sh
cmake --preset x64-Debug-Cross
cmake --build _build/x64-Debug-Cross
```

Alternatively, after entering the nix develop shell and fetching xwin, you can start CLion from the same shell and
work on the project there.

The commands above assume you have Nix installed and available. If you do not, have a look at `flake.nix` for what
dependencies you need.

## Contributing

If you'd like to contribute to the SDK, feel free to open a pull request or an issue.
