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

Version 4.0.6
- Ogg Opus files (.opus, or .ogg holding Opus) are read and saved like Ogg Vorbis: AUDIOAnalyzeFileW returns 6 (OGG),
  the OGG and AUDIO tag functions read and write their comments (OpusTags, of any size, on as many pages as they
  need; the audio pages and the OpusHead are left as they are), the duration leaves out the pre-skip and the
  sample rate is 48000, as Opus decodes. They read as no file at all before.
- OGGGetCodecW (new): "Vorbis" or "Opus" for the Ogg file analyzed last, empty for any other format. Wrappers:
  OGGCodec.
- Ogg Vorbis durations keep their fraction of a second (an integer division cut them to whole seconds).

Version 4.0.5
- File names of any length: a path of 246 characters or more is passed to Windows in its extended form (\\?\C:\...,
  \\?\UNC\server\...). AUDIOAnalyzeFileW returned 0 for a path of MAX_PATH (260) or more and every save failed
  (error 209). Shorter paths are passed as before.
- WMA title, author, copyright, description and rating are cut at 32766 characters, the most their 16-bit length
  holds: a longer one wrapped the length around and the fields after it were read from inside it.
- After a save or remove "ToFile" (or MPEGSet*W) on a file other than the analyzed one, the analyzed file keeps its
  own ID3v2/ID3v1 sizes: its duration, bit rate and MD5 range were figured with the other file's sizes until it
  was analyzed again.
- WAV tags saved into another file (FLACSaveChangesToFileW and WAVSaveChangesToFileW since 4.0.4.7) carry the
  broadcast chunks (bext, cart) whole too, not only INFO and DISP.
- WMA Metadata Library items (values over 64 KB) are saved in a file without a Header Extension: one is added.
  They were dropped.
- A save "ToFile" into a file other than the analyzed one uses that file's own ID3v1 size, and no read takes more
  than the bytes left in the file: MP4SaveChangesToFileW into a file of under 128 bytes figured a size of about
  4 GB from the analyzed file's ID3v1 tag, and the 32-bit build threw out of the DLL (found by the x86 API fuzz).

Version 4.0.4.9
- ID3v1 and Lyrics3 text ends at its first NUL (the NUL padding of an ID3v1 field was returned too, and a v1.1
  comment ended with the track number) and is read in the code page it was written in, as ID3v2 since 4.0.2
  (Hebrew from a Hebrew PC read as accented Latin elsewhere).
- GRP1 (iTunes grouping), MVNM and MVIN (movement name, number) are text frames: ID3V2GetTextFrameW reads them.
- An M4A save that rebuilds the file keeps an ID3v1 tag at its end (only the in-place save kept it).
- A WMA rebuild writes the new file size into the File Properties object.
- Picture links ("-->") read from a tag aren't opened on a drive letter mapped to a network share either.
- ID3V2GetAddendumTitleW/DescriptionW read the chapter's TIT2/TIT3 sub-frames (a title set with
  ID3V2SetTextSubFrameW read as the old one).
- MPEGSetCopyrightedW/OriginalW/PrivateW accept .MSF files, as AUDIOAnalyzeFileW does.
Wrappers: SafeStr reads up to the first NUL again (PtrToStringUni) and still frees the string.

Version 4.0.4.8
Stability check of 4.0.4.7 by fuzzing, x64 and x86: 60,000 damaged files (every format, 1 to 8 changes each,
analyzed, every getter read, saved) and 13,500 runs of 120 random API calls with hostile arguments (NULL and
300 K strings, out-of-range indexes and IDs, short buffers): no crash or hang. One defect found and fixed:
- M4A/MP4: every save leaked about 1.4 KB (the atom classes had no virtual destructor, so an atom deleted through
  its base class kept its own members, such as the mdat's source file name). Memory now stays flat over thousands
  of saves in every format; no file handles are left open.

Version 4.0.4.7
A review of the whole source. Saves that could lose audio, tags or the file, and crashes on damaged files:
All formats
- AUDIOAnalyzeFileW that fails (file locked, missing) no longer leaves the previous file as the target: a following
  *SaveChangesW wrote the cleared tags into the previous file.
- AUDIOGetVersionW, AUDIOGetDurationW and the other AUDIO getters no longer crash before the first analyze.
- FLAC, WAV, WMA and MP4 rebuilds check every read and write and replace the file only when the copy is complete
  (as ID3v2 and OGG since 4.0.2); a full disk or a dropped network share left a cut file or none.
- FLACSaveChangesToFileW, WAVSaveChangesToFileW and AUDIOSaveChangesToFileW for FLAC/WAV into a file other than the
  analyzed one (a copy encoded again from it) keep that file's own blocks/chunks and audio, and write the analyzed
  file's comments and pictures (FLAC) or INFO/DISP text (WAV). The analyzed file's layout was written over it: a
  FLAC copy no longer decoded, a WAV copy got the source's fmt chunk (e.g. 16-bit over 24-bit audio). The analyzed
  file must be of the same format (else error 217).
ID3v2
- Frames the library doesn't know (GRP1, MVNM, MVIN, PCST...) are kept on save; they were dropped.
- Chapter (CHAP/CTOC) titles and descriptions are kept on save; every save removed them.
- AUDIOGetCommentW reads the comment without a description (else the first one that isn't iTunes data); it read
  the first COMM (often iTunNORM) and the save copied that hex text into the visible comment. An empty comment
  removes it.
- AUDIOGetYearW/AUDIOSetYearW use TDRC in v2.4 tags (the year read empty and a set year was lost).
- AUDIOSaveChangesW no longer adds empty TALB/TPE1/TCON/TRCK/TCOM frames (written without a BOM, other readers
  reported them as damaged); an empty value removes the frame.
- A file without an ID3v2 tag is saved in the default format, not in the last analyzed file's tag version.
- v2.3 tags with unsynchronisation are read whole (frames lost bytes); v2.4 frame flags are written to match the
  bytes; encoding $02 is UTF-16 big endian; extended header and v2.4 footer sizes are right (the extended header was
  read as frames and 10 bytes of audio could be cut).
- ID3V2SetFormatAndEncodingW refuses values out of range (format 4 wrote a broken tag).
- ID3V2GetTextFrameW/ID3V2GetURLFrameW on a frame that isn't text/URL (PCNT, APIC...) return empty instead of
  crashing; the setters refuse such IDs.
- No crash on: UTF-16 text over 512 KB, a frame shorter than its data length indicator, a v2.3 frame size of
  FFFFFFFF, a damaged chapter sub-frame, a cue file with a short INDEX line or a repeated TRACK; a CHAP passed to
  the CTOC child functions; a TOC that lists itself.
- ID3V2ImportCueFileW: CUE frames are 1/75 s (were 1/76) and UTF-8 cue files read right.
- "-->" picture links in a tag are not opened when they point to a network path (\\server\...), nor over 64 MB.
- Smaller: POPM without a counter, SYLT empty lines, "(RX)"/"(CR)" genres read as Blues, RVA2/EQU2 converted
  between versions as garbage, chapter edits lost on a second save, a new CTOC's root flag random.
FLAC / OGG / APE / Lyrics3
- A FLAC save after a rebuild (adding a cover) without a new analyze no longer puts the old metadata in front of
  the audio; damaged metadata blocks refuse the save instead of overwriting audio; a zero-length block no longer ends
  the metadata; damaged PICTURE blocks no longer crash the read.
- FLACAddPictureArrayW writes the picture's MIME type and data and returns -1 on success (it returned 0, and the
  picture could be saved as an empty link).
- Vorbis comments (FLAC and OGG): a key with several values no longer grows on every save ("Rock|Pop|Pop..."),
  values over 256 KB (OGG cover art) and non-UTF-8 values are no longer read empty and deleted.
- A damaged Lyrics3 tag no longer makes a save or remove cut the file to 128 bytes, nor hangs the read.
- APEv1 tags: an edited value is stored as text, not raw UTF-16, and ANSI values are converted to UTF-8.
- Monkey's Audio "Insane" compression no longer reads out of the table; WavPack custom sample rates no longer do.
MP4 / M4A
- Files with several mdat atoms or fragments (moof), 64-bit atom sizes, or damaged atoms refuse to save instead of
  losing audio; an atom size 2..7 no longer crashes the read.
- A file without an ilst keeps its udta (chapters, GPS) on save; vmhd and QuickTime-style meta keep their data.
- MP4SetTextFrameW on trkn/gnre/covr no longer empties the field (covr then crashed).
- Integer atoms (tmpo, stik, cnID, plID...) are written as integers; track/disk numbers over 255 are kept.
- mdhd version 1 durations; duration of files with several sound tracks is the longest, not the sum.
WMA / WAV / MPEG
- WAV: a data chunk size of 0 or FFFFFFFF (recordings cut short, ffmpeg pipes) no longer makes a save write the file
  without its audio; a WAV over 2 GB keeps its audio; BlockAlign 0 no longer crashes AUDIOGetDurationW; ID3v2/ID3v1
  around the RIFF are kept; Hebrew INFO text is written in code page 1255 on a PC whose code page can't hold it.
- WMA: the header object count matches the objects written (a file could become unplayable); damaged descriptor
  sizes no longer overflow the heap; long values move to the Metadata Library instead of a cut 16-bit length;
  clearing every extended field works.
- MPEGSetCopyrightedW/OriginalW/PrivateW only write into MPEG files, at a checked frame header.
Wrappers
- C#: WAVSaveChangesW returns short (it was declared IntPtr: on x64 a failed WAV save could read as success);
  WAVSetCartChunkEntryW returns nothing. VB: ID3V2SetAddendumDescriptionW/TitleW return Short, not Integer.
- C# and VB: every string getter frees the BSTR the DLL returns (SafeStr: PtrToStringBSTR + FreeBSTR); each call
  leaked it, which added up over long library scans.

Version 4.0.4.6
- An APE item whose size is near 2 GB no longer crashes the read: the size check wrapped around in 32 bits, passed,
  and a 2 GB value was allocated (an exception out of the DLL ends the calling app on x86).
- An APE tag before a Lyrics3 tag is read (it read as no APE tag), and a save replaces it, moving the Lyrics3 tag
  down (a second APE tag was added after the Lyrics3 tag).
- A file without an ID3v1 tag no longer takes the ID3v1 size of the last file read with one (an APE save on it
  looked for the footer 128 bytes early and cut 128 bytes off the end).
- A damaged APE item read no longer leaks it.
Version 4.0.4.5
- APESaveChangesToFile and APERemoveTag check the APE footer's size before cutting: below 32 bytes (a size of
  2 GB or more read as negative) or reaching the start of the file with the ID3v1 tag, the tag is damaged and
  nothing is cut (error 218). A negative size grew the file with zeros, up to gigabytes, in 4.0.4.4 and earlier.
- Reading a damaged APE tag no longer hangs: an item key running to the end of the file looped for ever, and a
  footer size that can't fit the file is now taken as no APE tag.
Version 4.0.4.4
- A damaged MP4 freeform ("----") atom no longer hangs the read: a huge box size wrapped the bounds check
  around in the 32-bit build, and the walk over the atom's boxes never ended.
- MP4GetiTuneFrame/MP4SetiTuneFrame match a freeform atom by its name under the com.apple.iTunes naming
  authority only (4.0.4.3 matched any mean, so an atom of the same name under another one was replaced or removed).
- MP4SetiTuneFrame with an empty text removes every atom of that name (only the first was).
- A freeform atom's text ends at its first NUL again.
- APESaveChangesToFile stops when the APE footer's size reaches past the start of the file (a damaged tag):
  it cut nothing and wrote the new tags after the old ones.
Version 4.0.4.3
MP4 freeform ("----") atoms, such as iTunes custom fields (----:com.apple.iTunes:NAME):
- MP4SetiTuneFrame with an empty text removes the atom. It removed nothing: the removal compared the atom's
  path alone, which never names a freeform atom.
- MP4GetiTuneFrame and the atom's name are read by walking its mean/name/data boxes. They were read at fixed
  offsets, right only when the mean was exactly "com.apple.iTunes"; another naming authority read garbage.
Version 4.0.4.2
APE tag saves on MP3 files no longer cut the end of the audio. Saving an APE tag over an existing one in a file
that also has an ID3v1 tag removed the old APE and ID3v1 tags, then cut 128 more bytes for the ID3v1 tag it had
already removed: every such save took 128 bytes off the last audio frame. The ID3v1 tag is now cut there only when
there was no APE tag to remove.
Version 4.0.4.1
M4A/MP4 tag saves no longer break the audio of files without padding after the moov atom (ffmpeg's -movflags
faststart writes them that way), nor of files whose moov comes after the audio.
- A save whose new tag was smaller than the old one, in a file without padding, put a free atom 8 bytes larger than
  the space given up in front of the audio: the audio moved 8 bytes while the chunk offsets (stco) stayed, so every
  frame was read 8 bytes off. The free atom now fills exactly the space given up (a gap of 1 to 8 bytes, too small
  for one, rebuilds the file instead).
- The padding was always put in front of the audio, so in a file with the moov after the audio (as ffmpeg writes
  without faststart) a save moved the audio and not its offsets. Padding now goes right after moov, wherever moov is.
- A rebuilt file (the tag doesn't fit) moves the chunk offsets of every track, not only the first, and the 64-bit
  offsets (co64) of large files too.
Version 4.0.3
OGG Vorbis tags of any size are read and saved. Up to 4.0.2 the comments had to fit in the stream's second Ogg page:
- Saving longer comments (a cover in METADATA_BLOCK_PICTURE, long lyrics) overran the page's lacing table, which
  crashed the process or wrote a file players reject.
- Reading comments that spanned pages (as ffmpeg, foobar2000 or Mp3tag write a cover) read the page headers in
  between as text, so later fields were garbled or lost.
Now the comment and setup packets are read whole across their pages and written back on as many pages as they need;
the audio pages after them are renumbered (and their checksums set) when the header's page count changes. The copy
replaces the original in one step only when every read and write succeeded, as MP3 saves do since 4.0.2. A stream
whose header pages can't be replaced on their own (another logical stream between them, or audio on the setup
packet's last page) is not saved and reports an error.

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
- Text in frames marked ISO-8859-1 is read in the encoding it was actually written in: valid UTF-8 as UTF-8,
  words of Hebrew letters as Windows-1255, single accented letters and symbols as Windows-1252, other words in the
  PC's code page. It used to be read in the PC's code page only (Hebrew showed as accented Latin on most PCs, and
  "Beyoncé" got a Hebrew letter on a Hebrew PC).

Version 4.0.1
ID3v2 saves keep the tag's own version and store text in Unicode. Up to 4.0 the save format and text encoding were
one process-wide setting that started at ID3v2.3 with ISO-8859-1 and stayed at whatever was last set: saving a v2.4
tag converted it to v2.3 and dropped its v2.4-only frames (TDRC, the date), every text frame was rewritten as
ISO-8859-1 (non-Latin text such as Hebrew became "?"), and a format set for one file applied to every later save.
Now, unless ID3V2SetFormatAndEncoding is called after the file is analyzed, a save uses the version the tag was read
with (UTF-8 for v2.4, UTF-16 with BOM for v2.2/v2.3); a file without a tag gets the last format set explicitly
(v2.3 by default). An explicit ID3V2SetFormatAndEncoding applies to the saves of the file now loaded.
The file and product version is now 4.0.1.0 (4.0 still reported the 2.0.4.0 it inherited from AudioGenie2).
