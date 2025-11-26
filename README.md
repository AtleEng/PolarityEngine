# Polarity Engine

![C++](https://img.shields.io/badge/C%2B%2B-20-orange)
![Platform](https://img.shields.io/badge/platform-Windows-blue)
[![License: MIT](https://img.shields.io/badge/License-Apache-yellow.svg)](./LICENSE)

Polarity Engine is a lightweight, cross-platform 2D game engine for low-level gamedev

- [Features](#features)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Dev Information](#dev-information)
- [Roadmap](#roadmap)
- [Additional Resources](#additional-resources)
- [Credits](#credits)
  
---

## Features

Right now the engine provides abstractions for input, and 2d texture rendering on only windows

---

## Project Structure

Brief explanation of key folders/files:

- `Polarity` - Engine files
- `Sandbox` - Example project

<details>
	 <summary><strong>Engine File Tree</strong></summary>
	
```text
Polarity
├───assets
├───src
│   ├───engine
│   │   ├───core
│   │   ├───events
│   │   ├───imGui
│   │   ├───math
│   │   ├───renderer
│   │   └───utils
│   └───platform
│       ├───openGL
│       └───windows
└───thirdparty
```

</details>

---

## Getting Started
The engine should be implemented as a submodule:  
```text
Project folder        <-- Have the same name as your game  
├── PolarityEngine/   <-- Engine as submodule (+Sandbox, Tools, Thirdparty)  
└── TestGame/         <-- Your game (Copied version of Sandbox)  
```

### Setup
- Clone the repository as a submodule:
```text
git submodule add https://github.com/AtleEng/PolarityEngine
git submodule update --init --recursive
```
- Add a `premake5.lua` file in your Project folder for generating solution
- Copy `Sandbox` to your Project folder as a starting point
- Remove `premake5.lua` in PolarityEngine
- In your Project folder use:
 ```text
PolarityEngine\thirdparty\premake\premake5.exe vs2022
 ```
- Now a clean vs solution should have been generated

 <details>
	 <summary><strong>Inside project folder premake5.lua file</strong></summary>
	 
```text
-- This should be in your project folder

workspace "TestGame"--          <-- name of your game
    architecture "x64"
    startproject "TestGame"--   <-- name of your game

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

-- Output directory format
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

includedir = {}
includedir["GLFW"]      = "%{wks.location}/PolarityEngine/Polarity/thirdparty/GLFW/include"
includedir["Glad"]      = "%{wks.location}/PolarityEngine/Polarity/thirdparty/Glad/include"
includedir["ImGui"]     = "%{wks.location}/PolarityEngine/Polarity/thirdparty/imgui"
includedir["glm"]       = "%{wks.location}/PolarityEngine/Polarity/thirdparty/glm"
includedir["stb_image"] = "%{wks.location}/PolarityEngine/Polarity/thirdparty/stb_image"


group "Dependencies"
    include "PolarityEngine/Polarity/thirdparty/GLFW"
    include "PolarityEngine/Polarity/thirdparty/Glad"
    include "PolarityEngine/Polarity/thirdparty/imgui"
group ""


group "Core"
	include "PolarityEngine/Polarity"
group ""

group "Games"
    include "TestGame"--      <-- name of your game
group ""
```
</details>

 <details>
	 <summary><strong>Inside your game premake5.lua file</strong></summary>
	 
```text
-- This should be in your game folder
project "TestGame"--          <-- name of your game
    location "%{wks.location}/%{prj.name}"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }
        
    vpaths
    {
        ["Header Files"] = { "**.h" },
        ["Source Files"] = { "**.cpp" }
    }

    includedirs
    {
        "%{wks.location}/PolarityEngine/Polarity/src",
        "%{wks.location}/PolarityEngine/Polarity/thirdparty",
        "%{includedir.glm}"
    }

    links
    {
        "Polarity"
    }

    filter "system:windows"
    systemversion "latest"
        defines
        {
            "POLARITY_PLATFORM_WINDOWS",
            "POLARITY_ENABLE_ASSERTS",
        }

    filter "configurations:Debug"
        defines "POLARITY_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "POLARITY_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "POLARITY_DIST"
        runtime "Release"
        optimize "on"

```

</details>

### Update
To update Polarity in Project folder simply run:
- `cd PolarityEngine`
- `git pull`

Then remove the premake5.lua in `PolarityEngine`

---

## Dev information


 <details>
  <summary><strong>Dependencies</strong></summary>

  | Library     | Purpose              |
  |-------------|----------------------|
  | **OpenGL**  | Rendering backend    |
  | **GLFW**    | Windowing & input    |
  | **ImGui**   | Debug/Editor UI      |
  | **GLM**     | Math library         |
  | **stb_image** | Image loading     |

</details>

---

## Roadmap

### Core Systems

- Platform abstraction — *In Progress*
- Audio system — *Not started*
- ECS system — *Not started*

### Rendering
- OpenGL 2D renderer — *Basic prototype done*
- Batch rendering — *Not started*
- Texture atlas — *Not started*

### Tools
- Editor — *Not started*

### Platforms
- Linux — *Planned*
- macOS — *Planned*

---

## Additional Resources

- very empty right now...

---

## Credits
Made by:
- Atle Engelbrektsson
