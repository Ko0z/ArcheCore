# ArcheCore
A server software for Archeage written in C++
Much inspired from AAEmu, huge credit to them.
At this time there is no database integration so you cant save any progression. The only thing that is implemented thus far is the ability to log in, create a character and run around in the world with everyone else that has connected to the server. No combat, no mobs/npc etc..

# Dependencies

![image](https://github.com/Ko0z/ArcheCore/assets/9639004/3464ac0d-67dc-474d-a469-f6eefae30313)

Updated versions will still probably work fine in the future...

# How to compile after cloning

## With vcpkg manager

Open the terminal in a location where you want to store the vcpkg files and dependencies and enter these lines in cmd one line at a time:
```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat -disableMetrics

vcpkg install asio:x64-windows-static
vcpkg install entt:x64-windows-static
vcpkg install fmt:x64-windows-static
vcpkg install glm:x64-windows-static
vcpkg install nlohmann-json:x64-windows-static
vcpkg install pugixml:x64-windows-static
vcpkg install spdlog:x64-windows-static
vcpkg install sqlite3:x64-windows-static
```
**[ CHOICE A ]** - If you're fine with the dependencies being "visible" to all your visual studio projects...
```
vcpkg integrate install
```
In Visual Studio, select each project one by one in the Solution Explorer and go to Properties, then under "vcpkg" change "Use Static Libraries" to: Yes
Also, under C/C++ - Code Generation, change "Runtime Library" to Multi-threaded(/MT) and for Debug Config: Multi-threaded Debug(/MT)
Also, under General and C++ Language Standard:  C++17 or higher

DONE!

**[ CHOICE B ]** - If you want it to be project specific you can create a Nuget Package and link that to each project that needs them.
```
vcpkg integrate project
```
Copy the command from the cmd prompt starting with "Install-Package ...."

Go in to the Visual Studio solution, go to drop down "Tools" -> "Nuget Package Manager" -> "Package Manager Console"
Enter the command you copied from cmd prompt above
Make sure to run the command for each project by setting the "Default Project" drop down respectively.

Now select each project one by one in the Solution Explorer and repeat: go to Properties, then under "vcpkg" change "Use Static Libraries" to: Yes
Also, under C/C++ - Code Generation, change "Runtime Library" to Multi-threaded(/MT) and for Debug Config: Multi-threaded Debug(/MT)
Also, under General and C++ Language Standard:  C++17 or higher

DONE!

You may have to go back up to drop-down "Tools" -> "Nuget Package Manager" -> "Manage NuGet Packages for Solution..." and enable the packages

# How to run the server

Make sure that you have compiled the project and made a build so that you have the build folder with the executables for the LoginServer and WorldServer. If compiled in VisualStudio, it's usually found inside the folder "x64/release/" for example.

You have to copy the "data" folder and the "Server.cfg" file from the repository root and paste it in your build root folder.

![image](https://github.com/Ko0z/ArcheCore/assets/9639004/e5b65fbd-23c9-47a7-85fd-935e49298f43)

Then you have to find the serverside information data, a sqlite database containing information about everything from monster spawns to damage values. You can find the "compact.sqlite3" in the AAEmu git repository Wiki under "Getting Started" https://github.com/AAEmu/AAEmu/wiki (this is also where you can find a client if you dont have a 1.2 client already but it is not needed to run the server) **You need to place this "compact.sqlite3" file inside the "data" folder that you already placed in the root build folder.**

![image](https://github.com/Ko0z/ArcheCore/assets/9639004/5f639770-db46-42be-bc47-63fd97c761fa)

Configure the server Port and IP settings inside the Server.cfg file. (open with notepad or any IDE) If you plan to run the server over the internet you might have to open the ports in your router that you've chosen respectively. Running on LAN (127.0.0.1) obviously work fine without this step.

You can run the LoginServer independantly of the WorldServer on different machines if necessary. The LoginServer do NOT need the data folder in this case, but the "Server.cfg" file is needed by both executables and it needs to be in the same folder as their .exe

![image](https://github.com/Ko0z/ArcheCore/assets/9639004/326659ab-a760-4b8a-8842-c669221aed17)

# Future plans
- Change from ENTT to FLECS entity library to experiment with their entity relationship features
- Use the c++ library [recast_navigation](https://github.com/recastnavigation/recastnavigation) for doing navigation and pathfinding jobs
- Load and spawn mobs for cells where players are active by some spatial hashing system
- Handle skills and combat in general

to be continued...

Ko0z/ArcheCore is not in any way affiliated with XLGames. All brands and trademarks belong to their respective owners. ArcheCore is not a XLGames-approved software, nor is it associated with XLGames.
