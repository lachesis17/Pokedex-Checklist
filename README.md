# <img src="https://github.com/user-attachments/assets/2b44b8f4-73c1-4b0a-be92-710214aa8cfa" width="64" align="absmiddle"> **Pokedex Checklist**

#### Download Options:

- Both the installer and portable exe can be found at [releases](https://github.com/lachesis17/Pokedex-Checklist/releases/)
- **Installer**: Download `PokedexChecklistInstaller.exe` for automatic installation with Start Menu shortcuts
- **Portable**: Download `PokedexChecklist.exe` to run directly without installation
- **Save File Directory**: `C:/Users/<your_user>/AppData/Roaming/Pokedex Checklist/`

_Note: Windows warns for unsigned executables (and aint no way I'm paying for that)._
_Click "More info" → "Run anyway"._

#### Dependencies:

- [Qt 6.8](https://github.com/qt/qtbase): Qt Base for QtWidgets, QtCore and QtGui
- [RapidJSON](https://github.com/Tencent/rapidjson): Tencent RapidJSON
- [CMake](https://cmake.org/download/): CMake (using mingw/gcc as the compiler) or use [CMake Tools VSCode Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

#### Compiling

CMake handles everything for building as long as you have your dependencies.

CMake will try to auto-detect your Qt installation first. If that doesn't work, just uncomment and update the `CMAKE_PREFIX_PATH` line in `CMakeLists.txt` to whatever your path is, something like: `C:/Qt/6.8.3/mingw_64/lib/cmake`

This released version is built with static libraries from the [Qt Base repository](https://github.com/qt/qtbase) using CMake, but you could just [install Qt the regular way.](https://www.qt.io/download-qt-installer)

The installer is compiled using [NSIS](https://nsis.sourceforge.io/). If you want to build the installer, just move both the built exe file and `/assets/logo.ico` into `/packaging/` and run `installer.nsi` using NSIS.

<br>
<details>
<summary>Licenses</summary>

###### The source code is provided in compliance with LGPLv3 for compiling static libraries using Qt 6.8.

###### _All Pokémon likenesses, names, and trademarks are the property of Nintendo, Creatures Inc., and GAME FREAK Inc._

</details>
