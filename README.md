# Poster EXR

This binary module adds a function `render.WriteEXR(n)`, which takes a scale (only tested for n = 1) and outputs an EXR when in the 2D rendering context.

### Build Instructions

This was built in and tested in Windows 10 using Visual Studio 2022

1. `git clone` this repo and initialize the garrysmod_common submodule
2. Switch the branch of garrysmod_common to the x86-64-branch, and then initialize the submodules within it
3. Add premake5.exe into the same folder as premake5.lua
4. Run the appropriate premake command for your operating system (make a Visual Studio 2022 project file)
5. Build it in your operating system, with the Release configuration (or similar for your operating system)
6. Move `gmcl_poster_exr_win64.dll` (or whatever) into your `garrysmod/lua/bin` folder

### Example Usage

```lua
require("poster_exr")

local function posterEXR(_, _, args, _)
	if not args[1] then
		MsgC(color_white, "Usage: poster_exr <scale>")
		return
	end
	local scale = tonumber(args[1])
	if scale <= 0 then
		MsgC(color_white, "Scale must be a positive, nonzero integer")
		return
	end
	
	
	cam.Start2D()
	
	render.RenderView()
	render.WriteEXR(scale)
	
	cam.End2D()
end
concommand.Add("poster_exr", posterEXR, nil, "Write an EXR file of a specified scale")
```

### Rational

While developing this was a learning experience for me, I unfortunately could not find any use for this for my animations. It takes a considerable time to write to an EXR file, and that time sets a lower bound for the delay to render frames. It also requires the user to put the `render.WriteEXR(n)` function into a render hook if one wanted to capture a Soft Lamps render. 

It would be neat to only necessitate a `poster_exr` concommand which does not require the lua state to set `render.RenderView`, but I couldn't find an effective way to interface with IRenderView to make this happen :/

The user of this may enjoy a 16-bit transformed image, but mileage may vary.

### Credits

- TinyEXR
- garrysmod_common
