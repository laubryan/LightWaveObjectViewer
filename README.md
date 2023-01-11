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

## Caveats

The tool still needs more work, but I'll improve it slowly over time.

- Only vertex and polygon information is extracted. Everything else is skipped.
- Polygons with more than 4 vertices are not loaded for now.
- The tool works on my small collection of object files, but there may be issues with other files that I haven't discovered yet.
- Only the LWO2 object file version is supported. LWO3 objects won't load for now.

## Future Work

As I mentioned, the tool needs a bit of work. Some improvements I'd like to make next:

- Handle n-sided polygons.
- Apply LightWave surface attributes (e.g. color, reflectivity, etc) and improve the shader.
- Allow user to interact with view.
- Additional UI to tweak the display.
- Some code refactoring.
