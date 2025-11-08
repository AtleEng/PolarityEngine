# Polarity Engine

### A lightweight 2D engine
The Polarity Engine is a lightweight, cross-platform 2D game engine built to give game designers maximum control over how their games feel and behave. It provides core abstractions for rendering, input handling, and platform management, allowing developers to focus on creating unique, low-level game systems without wasting time reinventing the common foundations.

---

## Project Structure

```text
Polarity
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

---

## Development Info

### How to Build
- Run generateProj.bat
- Build via Visual Studio C/C++

### How to use
- Add a new project to the Polarity solution
- Add a cpp file in the new project

Inside file:
```text
#include <Polarity.h>
```


<details>
  <summary>Dependencies</summary>
OpenGL - GLFW - ImGui - GLM
</details>

---

## Roadmap

Basic Rendering and abstractions -> shaders and materials -> full 2D rendering

---

## License

License type: Apache (see LICENSE file)

---

## Credits
### Made by:
- Atle Engelbrektsson
