⚙️ 1. Install SFML
🪟 Windows (Visual Studio or MinGW)
Option 1 — Using MSYS/MinGW

Download SFML 2.5.1 for MinGW from:
🔗 https://www.sfml-dev.org/download/sfml/2.5.1/

Extract it to a folder like:
C:\SFML

Make sure MinGW or g++ is installed (e.g. from Code::Blocks or MSYS).

Option 2 — Using Visual Studio

Download the Visual C++ 15 (2017) version of SFML.

Extract it somewhere like C:\SFML.

Visual Studio can link it automatically when building with CMake.

🐧 Linux (Ubuntu / Debian / Arch / Fedora)

Run:

sudo apt update
sudo apt install libsfml-dev cmake g++

🍎 macOS

Run:

brew install sfml cmake

📦 2. Add a Font

Place any .ttf font in the assets/ folder.
Example: download a free font like “OpenSans-Regular.ttf”
and rename it to:

assets/font.ttf

🧰 3. Build the Project with CMake

Open a terminal in the project folder and run these commands:

mkdir build
cd build
cmake ..
make

✅ After successful build:

You’ll get an executable file (depends on your OS):

Windows: tictactoe.exe

Linux/macOS: ./tictactoe

▶️ 4. Run the Game
./tictactoe


or double-click tictactoe.exe on Windows.

You’ll see:
🕹️ A window titled Tic Tac Toe with a 3×3 grid.
Click to place Xs and Os, or press:

R → Restart

M → Toggle AI / Human mode

🧩 5. (Optional) Run from Visual Studio

If you prefer Visual Studio:

Open Visual Studio.

Choose:
File → Open → CMake → Open Folder → Select the project root.

Wait for CMake to configure.

Press Build → Build All.

Press Run ▶️.

🧠 Common Errors
Error	Fix
cannot find -lsfml-graphics	Install libsfml-dev or set SFML_DIR path
Missing font	Make sure assets/font.ttf exists
Window closes instantly	Run from terminal to see error messages
