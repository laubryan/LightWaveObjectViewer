# LightWaveObjectViewer

## Description

Tool to display LightWave 3D objects.

LightWave used to be much more popular than it is today, but I still have a small collection 
of object files from that era that I wanted to quickly view without importing them into 
something more complicated like Unity or some conversion app. Thus, the LightWave Object Viewer was born.

![LightWave Object Viewer window](https://github.com/laubryan/LightWaveObjectViewer/blob/187a66ca4ecc103661771be7c60d40bc36f634b8/Screenshots/Main%20Screenshot.jpg?raw=true)

LightWave Object Viewer is a Windows application created in Visual Studio.


## Usage

Compile and run the tool, and then you can simply drag and drop a LightWave object (LWO) 
file onto the window to display it.

## Caveats

The tool still needs more work, but I'll improve it slowly over time.

- Only vertex and polygon information is extracted. Everything else is skipped.
- Polygons with more than 4 vertices are not loaded for now.
- The tool works on my small collection of object files, but there may be issues with other files that I haven't discovered yet.
- Only the LWO2 object file version is supported. LWO3 objects won't load for now.

## License

### MIT License

Copyright (c) 2023 Bryan Lau

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
