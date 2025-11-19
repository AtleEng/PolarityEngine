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
- `generateProj.bat` - build solution for VS

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

### Setup
- Add a new project to the Polarity solution
- Add a cpp file in the new project
	
## Inside cpp file

```text
#include <Polarity.h>

class Sandbox : public Polarity::Application
{
public:
	Sandbox()
	{
		  //Entry point
	}
	~Sandbox(){}
};

Polarity::Application* Polarity::CreateApplication()
{
	return new Sandbox();
}
```

How to Build

- Run generateProj.bat
- Build via Visual Studio C/C++

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

Base functionality -> Rendering -> shaders and materials -> 2D rendering -> filesystems

---

## Additional Resources

- very empty right now...

---

## Credits
Made by:
- Atle Engelbrektsson
