# Poster EXR

This binary module adds a function render.WriteEXR(n), which takes a scale (only tested for n = 1) and outputs an EXR when in the 2D rendering context.

While developing this was a learning experience for me, I unfortunately could not find any use for this for my animations. The user of this may enjoy a 16-bit transformed image, but mileage may vary.

This was built in and tested in Windows 10 using Visual Studio 2022

### Build Instructions

1. `git clone` garrysmod_common and switch to its x86-64 branch. Do the same for the submodules
2. 

### Example Usage

```
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
```lua



### Credits

- TinyEXR
- garrysmod_common
