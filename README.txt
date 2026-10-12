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

Version 4.0.2
Saves can no longer lose the audio or values that weren't changed.
- A save that rebuilds the file (the new tag doesn't fit in the old one) checks every read and write, makes sure the
  copy holds all of the audio, and then replaces the original in one step. Up to 4.0.1 a read or write error (disk
  full, a network share dropping) ended the copy early, the original was deleted and the truncated copy renamed over
  it; if that rename failed, the audio was left only in "name~".
- Only frames that are new or were changed, or whose encoding the saved version doesn't allow (UTF-16BE and UTF-8
  before v2.4), are encoded again. Every text frame used to be decoded and re-encoded, which kept only the first value
  of a v2.4 multi-value frame and emptied a frame that wasn't valid UTF-8. Untouched frames keep their bytes.
- The DLL no longer dispatches the caller's window messages while it works. A message handler that called the DLL
  again replaced the current file in the middle of a read or save (a tag saved into the wrong file) or closed the
  outer save's file streams (a truncated file). A caller that needs a responsive window calls the DLL from a worker
  thread.
- UTF-16 big-endian strings no longer lose their last characters.

Version 4.0.1
ID3v2 saves keep the tag's own version and store text in Unicode. Up to 4.0 the save format and text encoding were
one process-wide setting that started at ID3v2.3 with ISO-8859-1 and stayed at whatever was last set: saving a v2.4
tag converted it to v2.3 and dropped its v2.4-only frames (TDRC, the date), every text frame was rewritten as
ISO-8859-1 (non-Latin text such as Hebrew became "?"), and a format set for one file applied to every later save.
Now, unless ID3V2SetFormatAndEncoding is called after the file is analyzed, a save uses the version the tag was read
with (UTF-8 for v2.4, UTF-16 with BOM for v2.2/v2.3); a file without a tag gets the last format set explicitly
(v2.3 by default). An explicit ID3V2SetFormatAndEncoding applies to the saves of the file now loaded.
The file and product version is now 4.0.1.0 (4.0 still reported the 2.0.4.0 it inherited from AudioGenie2).
