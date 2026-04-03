AUDIOGENIE4

This rebuild of AUDIOGENIE works with .Net Framework 4.8 and .Net Framework 4.8.1 using P/Invoke.
Wrapper files are included for C# and VB.NET.

This build has also been tested using .Net Core 10 with Visual Studio 2026.

The source can be configured and compiled into a 32-bit or 64-bit DLL.

Like earlier versions (AudioGenie2.dll, AudioGenie3.dll) the core library has a single instance limitation, so
it is not suitable for multi-threaded parallel processing.

It is worth noting that it was originally written using base 1 arrays instead of base 0. I didn't change it for backward code compatibility.

Note:
This library is far superior to TagLibSharp when dealing with ID3v1, ID3v2.3, ID3v2.4, and APE tags on MP3 files. 
TagLibSharp uses a dangerous anti-pattern of metadata consolidation and responds to requests for tag data by giving results for other tags.
This regularly results in garbage tag data or file corruption because different tags use different encodings (ANSI, UTF-8, UTF-16). 









