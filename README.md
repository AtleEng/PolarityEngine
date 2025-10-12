# Atlas Engine

### A lightweight 2D engine with OpenGL-based rendering and core system abstractions for custom, low-level game development.
The Atlas Engine is a lightweight, cross-platform 2D game engine built on OpenGL. It provides core abstractions for rendering, input handling, and platform management (not right now), allowing developers to focus on creating unique, low-level game systems without wasting time reinventing the common foundations. Atlas is made to be intentionally slim, giving game designers maximum control over how their games feel and behave.

---

## Project Structure

Brief explanation of key folders/files:

- `Atlas` - Engine files
- `build.bat` - build solution for VS
- `README.md` - Project overview

```text
Atlas/
|- src/
|  |- engine/          # Main engine
|  |  |- events/            
|  |  |- core/         # Core logic
|  |  |- render/
|  |  |- imGui/
|  | 
|  |- platform/        # Cross-platform (currently only Windows)
|     |- windows/  
|
|- thirdparty/         # Engine specific thirdparty
```

---

## Development Info

### How to Build
- Run build.bat
- Build via Visual Studio C/C++

### How to use
- Add a new project to the Atlas solution
- Add a cpp file in the new project

Inside file:
```text
#include <Atlas.h>

class Sandbox : public Atlas::Application
{
public:
	Sandbox()
	{
		  //Entry point
	}
	~Sandbox(){}
};

Atlas::Application* Atlas::CreateApplication()
{
	return new Sandbox();
}
```


<details>
  <summary>Dependencies</summary>
OpenGL - GLFW - ImGui - GLM
</details>

---

## Roadmap

Base functionality -> Rendering -> shaders and materials -> 2D rendering -> filesystems

---

## License

License type: Apache (see LICENSE file)

---

## Credits
...
### Made by:
- Atle Engelbrektsson