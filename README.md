# LightWaveObjectViewer

## Description

Tool to display LightWave 3D objects.

LightWave used to be much more popular than it is today, but I still have a small collection 
of object files from that era that I wanted to quickly view without importing them into 
something more complicated like Unity or some conversion app. Thus, the LightWave Object Viewer was born.

![LightWave Object Viewer window](https://github.com/laubryan/LightWaveObjectViewer/blob/187a66ca4ecc103661771be7c60d40bc36f634b8/Screenshots/Main%20Screenshot.jpg?raw=true)

LightWave Object Viewer is a Windows application using Direct3D 11, created in Visual Studio.


## Usage

Compile and run the tool, and then you can simply drag and drop a LightWave object (LWO) 
file onto the window to display it.


## Technical

### Overview

The viewer tool was written in C++ using the Win32 API for the base application and the Direct3D 11 API for the 3D parts.

### Object Files

The object file parsing was written entirely based on official LightWave 3D object file format [documentation](https://static.lightwave3d.com/sdk/2019/html/filefmts/lwo2.html) available online (as of this writing). This is my own 
interpretation of the file format, so errors are entirely possible. I'll squash any bugs that I find, but my collection 
of LightWave 3D objects is small and I don't have the software anymore to create new ones (hence this viewer).

The file format uses a chunk-style organization, not unlike the block format found in MP3 ID3v2 files. I currently skip 
over most chunk types but I hope to add support for more in the future.

LightWave 3D uses n-sided polygons, so I'm currently splitting quads into triangles and rejecting polygons with more than 
four vertices. Generalizing this to n-sided polygons should be straightforward and is on my short-term to-do list. I'm 
also calculating the normal at each vertex since LightWave doesn't include the normals as part of the file.

For the moment I'm splitting shared vertices into separate ones, but this is definitely something that I want to look at 
in a bit more depth, i.e. for interpolated normals


### 3D

The viewer uses a simple and typical Direct3D 11 rendering strategy, using index and vertex buffers to render triangles.

Transformation matrices are passed to the shaders using constant buffers, with vertex and normal transformations taking 
place in the vertex shader, and lighting calculations done in the pixel shader. At this early stage, the lighting is 
simply a diffuse Lambert shading model with ambient lighting, but without the specular component, i.e.:

&nbsp; &nbsp; &nbsp; I<sub>r</sub> = k<sub>a</sub> I<sub>a</sub> + k<sub>d</sub> I<sub>i</sub> &#183; (n &#183; l)

Adding other components such as specular reflections and texturing should be straightforward, and I'd guess most of the 
work will be on the file parsing side rather than D3D.

## Future Work

Some improvements I'd like to make next, in rough priority order:

- Handle n-sided polygons.
- Automatically scale the view/move camera based on object size.
- Some code refactoring.
- Handle more LightWave surface attributes (e.g. color, reflectivity, etc) and improve the shader.
- Allow user to interact with view.
- Additional UI to tweak the display.
- Support LWO3 and test on a wider range of objects.
