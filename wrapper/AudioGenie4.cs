// *******************************************************
// ************ AUDIOGENIE DLL WRAPPER FOR C# ************
// *******************************************************
// * This is a Wrapper for the AudioGenie4.dll           *
// * written by Stefan Toengi.                           *
// * Many thanks to Tobias Lindner for the original file *
// * Refactored for .Net Core 10 and use in RapidTagger  *
// * by William Harvey 2026.                             *
// *                                                     *
// * To use this class, copy this file in your project   *
// * and the AudioGenie4.dll in the %Windows%\%system32% *
// * (32bit OS) or %Windows%\SysWOW64 folder(64bit OS)   *
// * or in the application folder. Now you can use this  *
// * class like the example:                             *
// *                                                     *
// * using AudioGenie;                                   *
// * AudioGenie4.SetPopupKey(keyValue);                  *
// * AudioGenie4.AUDIOAnalyzeFile("test.mp3");           *
// * AudioGenie4.AUDIOTitle = "TEST";                    *
// * AudioGenie4.AUDIOSaveChanges();                     *

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace AudioGenie
{
    #region Public Enums

    public enum AudioFormatID : short
    {
        UNKNOWN = 0,
        MPEG = 1,
        WMA = 2,
        MONKEY = 3,
        FLAC = 4,
        WAV = 5,
        OGGVORBIS = 6,
        MPEGPLUS = 7,
        AAC = 8,
        MP4M4A = 9,
        TTA = 10,
        WAVPACK = 11
    }

    public enum PictureType : short
    {
        OTHER = 0,
        FILE_ICON = 1,
        OTHER_FILE_ICON = 2,
        COVER_FRONT = 3,
        COVER_BACK = 4,
        LEAFLET_PAGE = 5,
        MEDIA_LABEL = 6,
        LEAD_ARTIST = 7,
        ARTIST_PERFORMER = 8,
        CONDUCTOR = 9,
        BAND_ORCHESTRA = 10,
        COMPOSER = 11,
        LYRICIST = 12,
        RECORDING_LOCATION = 13,
        DURING_RECORDING = 14,
        DURING_PERFORMANCE = 15,
        MOVIE_VIDEO_SCREEN_CAPTURE = 16,
        A_BRIGHT_COLOURED_FISH = 17,
        ILLUSTRATION = 18,
        BAND_ARTIST_LOGOTYPE = 19,
        PUBLISHER_STUDIO_LOGOTYPE = 20
    }

    public enum PictureLink : short
    {
        AS_FILENAME = -1,
        AS_PICTURE = 0
    }

    public enum ID3V2FRAMES : uint
    {
        ID3F_AENC = 0x41454E43,
        ID3F_APIC = 0x41504943,
        ID3F_ASPI = 0x41535049,
        ID3F_CHAP = 0x43484150,
        ID3F_COMM = 0x434F4D4D,
        ID3F_COMR = 0x434F4D52,
        ID3F_CTOC = 0x43544F43,
        ID3F_ENCR = 0x454E4352,
        ID3F_EQU2 = 0x45515532,
        ID3F_EQUA = 0x45515541,
        ID3F_ETCO = 0x4554434F,
        ID3F_GEOB = 0x47454F42,
        ID3F_GRID = 0x47524944,
        ID3F_IPLS = 0x49504C53,
        ID3F_LINK = 0x4C494E4B,
        ID3F_MCDI = 0x4D434449,
        ID3F_MLLT = 0x4D4C4C54,
        ID3F_OWNE = 0x4F574E45,
        ID3F_PCNT = 0x50434E54,
        ID3F_POPM = 0x504F504D,
        ID3F_POSS = 0x504F5353,
        ID3F_PRIV = 0x50524956,
        ID3F_RBUF = 0x52425546,
        ID3F_RVA2 = 0x52564132,
        ID3F_RVAD = 0x52564144,
        ID3F_RVRB = 0x52565242,
        ID3F_SEEK = 0x5345454B,
        ID3F_SIGN = 0x5349474E,
        ID3F_SYLT = 0x53594C54,
        ID3F_SYTC = 0x53595443,
        ID3F_TALB = 0x54414C42,
        ID3F_TBPM = 0x5442504D,
        ID3F_TCMP = 0x54434D50,  //itunes
        ID3F_TCOM = 0x54434F4D,
        ID3F_TCON = 0x54434F4E,
        ID3F_TCOP = 0x54434F50,
        ID3F_TDAT = 0x54444154,
        ID3F_TDEN = 0x5444454E,
        ID3F_TDLY = 0x54444C59,
        ID3F_TDOR = 0x54444F52,
        ID3F_TDRC = 0x54445243,
        ID3F_TDRL = 0x5444524C,
        ID3F_TDTG = 0x54445447,
        ID3F_TENC = 0x54454E43,
        ID3F_TEXT = 0x54455854,
        ID3F_TFLT = 0x54464C54,
        ID3F_TIME = 0x54494D45,
        ID3F_TIPL = 0x5449504C,
        ID3F_TIT1 = 0x54495431,
        ID3F_TIT2 = 0x54495432,
        ID3F_TIT3 = 0x54495433,
        ID3F_TKEY = 0x544B4559,
        ID3F_TLAN = 0x544C414E,
        ID3F_TLEN = 0x544C454E,
        ID3F_TMCL = 0x544D434C,
        ID3F_TMED = 0x544D4544,
        ID3F_TMOO = 0x544D4F4F,
        ID3F_TOAL = 0x544F414C,
        ID3F_TOFN = 0x544F464E,
        ID3F_TOLY = 0x544F4C59,
        ID3F_TOPE = 0x544F5045,
        ID3F_TORY = 0x544F5259,
        ID3F_TOWN = 0x544F574E,
        ID3F_TPE1 = 0x54504531,
        ID3F_TPE2 = 0x54504532,
        ID3F_TPE3 = 0x54504533,
        ID3F_TPE4 = 0x54504534,
        ID3F_TPOS = 0x54504F53,
        ID3F_TPRO = 0x5450524F,
        ID3F_TPUB = 0x54505542,
        ID3F_TRCK = 0x5452434B,
        ID3F_TRDA = 0x54524441,
        ID3F_TRSN = 0x5452534E,
        ID3F_TRSO = 0x5452534F,
        ID3F_TSIZ = 0x5453495A,
        ID3F_TSO2 = 0x54534F32, //itunes
        ID3F_TSOA = 0x54534F41, //itunes
        ID3F_TSOC = 0x54534F43, //itunes
        ID3F_TSOP = 0x54534F50, //itunes
        ID3F_TSOT = 0x54534F54, //itunes
        ID3F_TSRC = 0x54535243,
        ID3F_TSSE = 0x54535345,
        ID3F_TSST = 0x54535354,
        ID3F_TXXX = 0x54585858,
        ID3F_TYER = 0x54594552,
        ID3F_UFID = 0x55464944,
        ID3F_USER = 0x55534552,
        ID3F_USLT = 0x55534C54,
        ID3F_WCOM = 0x57434F4D,
        ID3F_WCOP = 0x57434F50,
        ID3F_WOAF = 0x574F4146,
        ID3F_WOAR = 0x574F4152,
        ID3F_WOAS = 0x574F4153,
        ID3F_WORS = 0x574F5253,
        ID3F_WPAY = 0x57504159,
        ID3F_WPUB = 0x57505542,
        ID3F_WXXX = 0x57585858
    }

    public enum MP4FRAMES : uint
    {
        MP4_ALBUM = 0xA9616C62,          // ©alb
        MP4_ARTIST = 0xA9415254,         // ©ART
        MP4_AUTHOR = 0xA9617574,         //  ©aut
        MP4_ALBUMARTIST = 0x61415254,    // aART
        MP4_BEATSPERMINUTE = 0x746D706F, // tmpo
        MP4_CATEGORY = 0x63617467,       // catg
        MP4_CDSETNUMBER = 0x2064736B,    //  dsk
        MP4_DISCNUMBER = 0x6469736b,     // disk
        MP4_COMMENT = 0xA9636D74,        // ©cmt
        MP4_COMPOSER = 0xA9777274,       // ©wrt
        MP4_COMPILATION = 0x6370696C,    // cpil
        MP4_DAY = 0xA9646179,            // ©day
        MP4_COPYRIGHT = 0xA9637079,      // ©cpy
        MP4_CREATEDATE = 0xA9646179,     // ©day
        MP4_DESCRIPTION = 0xA9646573,    // ©des
        MP4_INFORMATION = 0xA9696E66,    // ©inf
        MP4_DIRECTOR = 0xA9646972,       // ©dir
        MP4_DISCLAIMER = 0xA9646973,     // ©dis
        MP4_ENCODER = 0xA9746F6F,        // ©too
        MP4_GROUPING = 0xA9677270,       // ©grp
        MP4_KEYWORD = 0x6B657977,        // keyw
        MP4_LYRICS = 0xA96C7972,         // ©lyr
        MP4_TITLE = 0xA96E616D,          // ©nam
        MP4_NETURL = 0xA975726C,         // ©url
        MP4_ORIGINALARTIST = 0xA96F7065, // ©ope
        MP4_ORIGINALFORMAT = 0xA9666D74, // ©fmt
        MP4_ORIGINALSOURCE = 0xA9737263, // ©src
        MP4_PERFORMER = 0xA9707266,      // ©prf
        MP4_PRODUCER = 0xA9707264,       // ©prd
        MP4_RATING = 0x72617465,         // rate
        MP4_WARNING = 0xA977726E         // ©wrn
    }

    public enum WAVFRAMES : uint
    {
        WAV_IARL = 0x4941524C, // ArchivalLocation
        WAV_IART = 0x49415254, // Artist
        WAV_ICMS = 0x49434D53, // Comissioned
        WAV_ICMT = 0x49434D54, // Comment
        WAV_ICOP = 0x49434F50, // Copyright
        WAV_ICRD = 0x49435244, // CreationDate
        WAV_ICRP = 0x49435250, // Cropped
        WAV_IDIM = 0x4944494D, // Dimension
        WAV_IDPI = 0x49445049, // DotsPerInch
        WAV_IENG = 0x49454E47, // Engineer
        WAV_IGNR = 0x49474E52, // Genre
        WAV_IKEY = 0x494B4559, // Keywords
        WAV_ILGT = 0x494C4754, // Lightness
        WAV_IMED = 0x494D4544, // Medium
        WAV_INAM = 0x494E414D, // Name
        WAV_IPLT = 0x49504C54, // PaletteSetting
        WAV_IPRD = 0x49505244, // Product
        WAV_IRTD = 0x49525444, // Rating
        WAV_ISBJ = 0x4953424A, // Subject
        WAV_ISFT = 0x49534654, // Software
        WAV_ISHP = 0x49534850, // Sharpness
        WAV_ISRC = 0x49535243, // Source
        WAV_ISRF = 0x49535246, // SourceForm
        WAV_ITCH = 0x49544348, // Technician
        WAV_ITRK = 0x4954524B  // Track
    }

    public enum WAVCARTENTRIES : short
    {
        WAV_CART_VERSION = 0,
        WAV_CART_TITLE = 1,
        WAV_CART_ARTIST = 2,
        WAV_CART_CUTID = 3,
        WAV_CART_CLIENTID = 4,
        WAV_CART_CATEGORY = 5,
        WAV_CART_CLASSIFICATION = 6,
        WAV_CART_OUTCUE = 7,
        WAV_CART_STARTDATE = 8,
        WAV_CART_STARTTIME = 9,
        WAV_CART_ENDDATE = 10,
        WAV_CART_ENDTIME = 11,
        WAV_CART_PRODUCERAPPID = 12,
        WAV_CART_PRODUCERAPPVERSION = 13,
        WAV_CART_USERDEF = 14,
        WAV_CART_LEVELREFERENCE = 15,
        WAV_CART_POSTTIMER0 = 16,
        WAV_CART_POSTTIMER1 = 17,
        WAV_CART_POSTTIMER2 = 18,
        WAV_CART_POSTTIMER3 = 19,
        WAV_CART_POSTTIMER4 = 20,
        WAV_CART_POSTTIMER5 = 21,
        WAV_CART_POSTTIMER6 = 22,
        WAV_CART_POSTTIMER7 = 23,
        WAV_CART_RESERVED = 24,
        WAV_CART_URL = 25,
        WAV_CART_TAGTEXT = 26
    }

    public enum SyncLyricContentTypes : short
    {
        UNGUELTIG = -1,
        SONSTIGES = 0,
        SONGTEXTE = 1,
        TEXTUMSCHREIBUNGEN = 2,
        BEWEGUNGEN = 3,
        EREIGNISSE = 4,
        AKKORDE = 5,
        BELANGLOSES = 6,
        URLS_ZU_WEBSEITEN = 7,
        URLS_ZU_BILDERN = 8
    }

    public enum SyncLyricTimeFormat : short
    {
        UNGUELTIG = -1,
        MPEGFRAMES = 1,
        MILLISEKUNDEN = 2
    }

    #endregion

        public static partial class AudioGenie4
    {
        private static string SafeStr(IntPtr ptr)
        {
            if (ptr == IntPtr.Zero) return string.Empty;
            return System.Runtime.InteropServices.Marshal.PtrToStringUni(ptr) ?? string.Empty;
        }
        public const string WM_ALBUMARTIST = "WM/AlbumArtist";
        public const string WM_ALBUMTITLE = "WM/AlbumTitle";
        public const string WM_AUTHOR = "WM/Author";
        public const string WM_AUTHORURL = "WM/AuthorURL";
        public const string WM_BEATSPERMINUTE = "WM/BeatsPerMinute";
        public const string WM_CATEGORY = "WM/Category";
        public const string WM_COMPOSER = "WM/Composer";
        public const string WM_CONDUCTOR = "WM/Conductor";
        public const string WM_CONTENTDISTRIBUTOR = "WM/ContentDistributor";
        public const string WM_CONTENTGROUPDESCRIPTION = "WM/ContentGroupDescription";
        public const string WM_DESCRIPTION = "WM/Description";
        public const string WM_DISCNUMBER = "WM/PartOfSet";
        public const string WM_ENCODEDBY = "WM/EncodedBy";
        public const string WM_ENCODINGSETTINGS = "WM/EncodingSettings";
        public const string WM_ENCODINGTIME = "WM/EncodingTime";
        public const string WM_GENRE = "WM/Genre";
        public const string WM_INITIALKEY = "WM/InitialKey";
        public const string WM_ISRC = "WM/ISRC";
        public const string WM_LANGUAGE = "WM/Language";
        public const string WM_LYRICS = "WM/Lyrics";
        public const string WM_MOOD = "WM/Mood";
        public const string WM_PARENTALRATING = "WM/ParentalRating";
        public const string WM_PARTOFSET = "WM/PartOfSet";
        public const string WM_PERIOD = "WM/Period";
        public const string WM_PRODUCER = "WM/Producer";
        public const string WM_PROMOTIONURL = "WM/PromotionURL";
        public const string WM_PROVIDER = "WM/Provider";
        public const string WM_PROVIDERCOPYRIGHT = "WM/ProviderCopyright";
        public const string WM_PROVIDERRATING = "WM/ProviderRating";
        public const string WM_PUBLISHER = "WM/Publisher";
        public const string WM_SDBRATING = "SDB/Rating";
        public const string WM_SHAREDUSERRATING = "WM/SharedUserRating";
        public const string WM_TITLE = "WM/Title";
        public const string WM_TOOLNAME = "WM/ToolName";
        public const string WM_TOOLVERSION = "WM/ToolVersion";
        public const string WM_TRACK = "WM/Track";
        public const string WM_TRACKNUMBER = "WM/TrackNumber";
        public const string WM_USERWEBURL = "WM/UserWebURL";
        public const string WM_WRITER = "WM/Writer";
        public const string WM_YEAR = "WM/Year";

        #region Native Methods

        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AACGetBitRateTypeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AACGetHeaderTypeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AACGetProfileW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short APEExistsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetAlbumW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetArtistW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetCommentW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetCopyrightW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetGenreW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetItemKeysW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int APEGetSizeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetTitleW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetTrackW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetUserItemW(string Key);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetVersionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr APEGetYearW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short APERemoveTagFromFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short APERemoveTagW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short APESaveChangesToFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short APESaveChangesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void APESetAlbumW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void APESetArtistW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void APESetCommentW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void APESetCopyrightW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void APESetGenreW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void APESetTitleW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void APESetTrackW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void APESetUserItemW(string Key, string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void APESetYearW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short AUDIOAnalyzeFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short AUDIOIsValidFormatW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetAlbumW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetArtistW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int AUDIOGetBitrateW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetChannelModeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int AUDIOGetChannelsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetCommentW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetComposerW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int AUDIOGetDurationMillisW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial float AUDIOGetDurationW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int AUDIOGetFileSizeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetGenreW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int AUDIOGetLastErrorNumberW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetLastErrorTextW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetLastFileW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetMD5ValueW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int AUDIOGetSampleRateW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetTitleW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetTrackW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetVersionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr AUDIOGetYearW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short AUDIOSaveChangesToFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short AUDIOSaveChangesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void AUDIOSetAlbumW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void AUDIOSetArtistW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void AUDIOSetCommentW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void AUDIOSetComposerW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void AUDIOSetGenreW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void AUDIOSetTitleW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void AUDIOSetTrackW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void AUDIOSetYearW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void SetPopupKeyW(string keyValue);
       [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int FLACGetBitsPerSampleW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial float FLACGetCompressionRatioW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr FLACGetItemKeysW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int FLACGetMaxBlockSizeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int FLACGetMaxFrameSizeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int FLACGetMinBlockSizeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int FLACGetMinFrameSizeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]        
        private static partial int FLACGetSamplesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr FLACGetUserItemW(string ItemKey);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr FLACGetVendorW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short FLACSaveChangesToFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short FLACSaveChangesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void FLACSetUserItemW(string ItemKey, string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void FLACSetVendorW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr GetAudioGenieVersionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int GetConfigValueW(int key);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr GetMD5ValueFromFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V1ExistsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V1GetAlbumW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V1GetArtistW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V1GetCommentW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V1GetGenreIDW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V1GetGenreItemW(short number);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V1GetGenreW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V1GetGenresW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V1GetTitleW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V1GetTrackW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V1GetVersionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V1GetYearW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V1RemoveTagFromFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V1RemoveTagW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V1SaveChangesToFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V1SaveChangesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V1SetAlbumW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V1SetArtistW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V1SetCommentW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V1SetGenreIDW(short nNewValue);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V1SetGenreW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V1SetTitleW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V1SetTrackW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V1SetYearW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddAudioEncryptionW(byte[] arr, int maxLen, string URL, short PreviewStart, short PreviewLength);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddAudioSeekPointW(byte[] arr, int maxLen, int start, int length, short numbers, byte BitsPerPoint);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddChapterW(string ID, string Title, string Description, int startTime, int endTime);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddChildElementW(string ParentTocID, string ChildID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddCommentW(string Language, string Description, string Text);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddCommercialFrameW(byte[] arr, int maxLen, string Price, string validUntil, string contactUrl, short receivedAs, string seller, string description);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddEncryptionW(byte[] arr, int maxLen, string URL, short Symbol);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddEqualisationW(byte[] arr, int maxLen, byte Interpolationmethod, string Identification);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddEventTimingCodesW(byte[] arr, int maxLen, byte TimestampFormat);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddGeneralObjectW(byte[] arr, int maxLen, string Mime, string FileName, string Description);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddGroupIdentificationW(byte[] arr, int length, string Url, byte symbol);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddLinkedInformationW(int FrameIdentifier, string URL, string additionalData);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddLyricW(string Language, string Description, string Text);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddMpegLocationLookupTableW(byte[] arr, int length, int Frames, int Bytes, int Milliseconds, byte BytesDeviation, byte MillisecondsDeviation);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddMusicCdIdentifierW(byte[] arr, int length);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddOwnershipW(string Price, string dateString, string Seller);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddPictureArrayW(byte[] arr, int maxLen, string Description, short PictureType);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddPictureFileW(string FileName, string Description, short PictureType, short asLink);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddPopularimeterW(string Email, short Rating, int Counter);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddPositionSynchronisationW(byte TimestampFormat, int Position);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddPrivateFrameW(byte[] arr, int length, string URL);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddRecommendedBufferSizeW(int BufferSize, byte EmbeddedInfoFlag, int Offset);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddRelativeVolumeAdjustmentW(byte[] arr, int length, string Identifier);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddReverbW(short reverbLeft, short reverbRight, byte bouncesLeft, byte bouncesRight, byte feedbackLeftToLeft, byte feedbackLeftToRight, byte feedbackRightToRight, byte feedbackRightToLeft, byte premixLeftToRight, byte premixRightToLeft);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddSignatureFrameW(byte[] arr, int length, short GroupSymbol);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddSyncLyricW(string Language, string Description, string Text, short ContentType, short TimeStampFormat);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddSynchronizedTempoW(byte[] arr, int length, short Format);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddTableOfContentW(string ID, string Title, string Description, short isOrdered);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddUniqueFileIdentifierW(byte[] arr, int length, string Owner);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddUserFrameW(string Language, string Text);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddUserTextW(string Description, string Text);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddUserURLW(string Description, string URL);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2DeleteAddendumW(string ID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2DeleteAllFramesW(ID3V2FRAMES FrameID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2DeleteChildElementW(string ParentTocID, string ChildID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2DeleteSelectedFrameW(ID3V2FRAMES FrameID, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2DeleteSubFrameW(string ID, ID3V2FRAMES FrameID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2ExistsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetAddendumDescriptionW(string ID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetAddendumTitleW(string ID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetAddendumTypeW(string ID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetAllFrameIDsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetAudioEncryptionDataW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetAudioEncryptionPreviewLengthW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetAudioEncryptionPreviewStartW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetAudioEncryptionURLW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetAudioSeekPointBitsPerIndexpointW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetAudioSeekPointDataW(byte[] arr, int maxLen);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetAudioSeekPointLengthW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetAudioSeekPointNumberW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetAudioSeekPointStartW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetChapterEndTimeW(string ID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetChapterStartTimeW(string ID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetChildElementIDW(string ID, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetChildElementsW(string ID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetCommentDescriptionW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetCommentLanguageW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetCommentW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetCommercialFrameContactURLW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetCommercialFrameDescriptionW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetCommercialFramePictureMimeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetCommercialFramePictureW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetCommercialFramePriceW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetCommercialFrameReceivedAsW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetCommercialFrameSellerNameW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetCommercialFrameValidUntilW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetEncodingW(ID3V2FRAMES FrameID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetEncryptionDataW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetEncryptionSymbolW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetEncryptionURLW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetEqualisationAdjustmentBitsW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetEqualisationDataW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetEqualisationIdentificationW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetEqualisationInterpolationW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetEventTimingCodesDataW(byte[] arr, int maxLen);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetEventTimingCodesTimestampFormatW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetFrameCountW(ID3V2FRAMES FrameID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetGeneralObjectDataW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetGeneralObjectDescriptionW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetGeneralObjectFilenameW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetGeneralObjectMimeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetGenreW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetGroupIdentificationDataW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetGroupIdentificationSymbolW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetGroupIdentificationURLW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetLinkedInformationAdditionalDataW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetLinkedInformationFrameIdentifierW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetLinkedInformationURLW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetLyricDescriptionW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetLyricLanguageW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetLyricW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetMpegLocationLookupTableBytesDeviationW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetMpegLocationLookupTableBytesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetMpegLocationLookupTableDataW(byte[] arr, int maxLen);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetMpegLocationLookupTableFramesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetMpegLocationLookupTableMillisecondsDeviationW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetMpegLocationLookupTableMillisecondsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetMusicCdIdentifierDataW(byte[] arr, int maxLen);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetOwnershipDateW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetOwnershipPriceW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetOwnershipSellerW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetPictureArrayW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetPictureDescriptionW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetPictureFileW(string FileName, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetPictureMimeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetPictureTypeTextW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetPictureTypeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetPlayCounterW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetPopularimeterCounterW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetPopularimeterEmailW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetPopularimeterRatingW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetPositionSynchronisationTimestampFormatW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetPositionSynchronisationValueW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetPossibleCHAPIDW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetPossibleTOCIDW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetPrivateFrameDataW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetPrivateFrameURLW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetRecommendedBufferSizeFlagW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetRecommendedBufferSizeOffsetW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetRecommendedBufferSizeValueW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetRelativeVolumeAdjustmentDataW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetRelativeVolumeAdjustmentIdentifierW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetReverbBouncesLeftW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetReverbBouncesRightW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetReverbFeedbackLeftToLeftW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetReverbFeedbackLeftToRightW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetReverbFeedbackRightToLeftW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetReverbFeedbackRightToRightW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetReverbLeftW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetReverbPremixLeftToRightW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetReverbPremixRightToLeftW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetReverbRightW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetSeekOffsetW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetSignatureFrameDataW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetSignatureFrameGroupSymbolW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetSizeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetSubFrameDescriptionW(string ID, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetSubFrameIDW(string ID, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetSubFrameImageTypeW(string ID, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetSubFrameImageW(byte[] arr, int maxLen, string ID, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetSubFrameTextW(string ID, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetSubFrameTypeW(string ID, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetSubFramesW(string ID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetSyncLyricContentTypeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetSyncLyricDescriptionW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetSyncLyricLanguageW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetSyncLyricTimeFormatW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetSyncLyricW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetSynchronizedTempoDataW(byte[] arr, int maxLen);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetSynchronizedTempoFormatW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2GetTOCIsOrderedW(string ID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetTOCRootIDW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetTextFrameW(ID3V2FRAMES FrameID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetURLFrameW(ID3V2FRAMES FrameID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetUniqueFileIdentifierDataW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetUniqueFileIdentifierOwnerW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetUserFrameLanguageW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetUserFrameW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetUserTextDescriptionW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetUserTextW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetUserURLDescriptionW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetUserURLW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr ID3V2GetVersionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2ImportCueFileW(String FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2RemoveTagFromFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2RemoveTagW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2SaveChangesToFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2SaveChangesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2SetAddendumDescriptionW(string ID, string newDescription);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2SetAddendumTitleW(string ID, string newTitle);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2SetChapterTimesW(string ID, int startTime, int endTime);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2SetFormatAndEncodingW(short format, short encoding);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V2SetGenreW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2SetImageSubFrameW(byte[] arr, int maxLen, string ID, string Description, PictureType pictureType);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V2SetPlayCounterW(int counter);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2AddSeekOffsetW(int offset);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2SetTOCIsOrderedW(string ID, short status);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V2SetTextFrameW(ID3V2FRAMES FrameID, string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2SetTextSubFrameW(string ID, ID3V2FRAMES FrameID, string textString, string description);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void ID3V2SetURLFrameW(ID3V2FRAMES FrameID, string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short ID3V2SetURLSubFrameW(string ID, ID3V2FRAMES FrameID, string urlString, string description);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short LYRICSExistsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr LYRICSGetAlbumW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr LYRICSGetArtistW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr LYRICSGetAuthorW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr LYRICSGetGenreW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr LYRICSGetImageLinkW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr LYRICSGetIndicationW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr LYRICSGetInformationW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr LYRICSGetLyricsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int LYRICSGetSizeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int LYRICSGetStartPositionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr LYRICSGetTitleW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr LYRICSGetVersionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short LYRICSRemoveTagFromFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short LYRICSRemoveTagW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short LYRICSSaveChangesToFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short LYRICSSaveChangesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void LYRICSSetAlbumW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void LYRICSSetArtistW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void LYRICSSetAuthorW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void LYRICSSetGenreW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void LYRICSSetImageLinkW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void LYRICSSetIndicationW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void LYRICSSetInformationW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void LYRICSSetLyricsW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void LYRICSSetTitleW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MONKEYGetBitsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial float MONKEYGetCompressionRatioW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MONKEYGetCompressionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int MONKEYGetFramesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial float MONKEYGetPeakW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int MONKEYGetSamplesPerFrameW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int MONKEYGetSamplesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void MP4DeleteEntriesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void MP4DeletePictureW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void MP4DeletePicturesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MP4GetTextFrameW(MP4FRAMES FrameID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MP4GetAllFrameIDsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MP4GetGenreW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MP4GetiTuneFrameW(string FrameID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int MP4GetPictureArrayW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int MP4GetPictureCountW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MP4GetPictureFileW(string FileName, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MP4GetPictureMimeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MP4GetTrackW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MP4GetVersionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MP4SaveChangesToFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MP4SaveChangesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void MP4SetGenreW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void MP4SetiTuneFrameW(string FrameID, string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MP4AddPictureArrayW(byte[] arr, int maxLen);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MP4AddPictureFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void MP4SetTextFrameW(MP4FRAMES FrameID, string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void MP4SetTrackW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MPEGGetEmphasisW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MPEGGetEncoderW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int MPEGGetFramePositionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int MPEGGetFrameSizeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int MPEGGetFramesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MPEGGetLayerW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr MPEGGetVersionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MPEGIsCopyrightedW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MPEGIsOriginalW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MPEGIsPaddingW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MPEGIsPrivateW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MPEGIsProtectedW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MPEGIsVBRW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MPEGSetCopyrightedW(string FileName, short newValue);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MPEGSetOriginalW(string FileName, short newValue);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MPEGSetPrivateW(string FileName, short newValue);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int MPPGetFramesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short MPPGetStreamVersionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetAlbumW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetArtistW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int OGGGetBitRateNominalW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetCommentW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetCopyrightW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetDateW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetDescriptionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetGenreW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetISRCW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetItemKeysW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetLicenseW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetLocationW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetOrganizationW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetPerformerW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetTitleW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetTrackW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetUserItemW(string ItemKey);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetVendorW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetVersionW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short OGGRemoveTagFromFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short OGGRemoveTagW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short OGGSaveChangesToFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short OGGSaveChangesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetAlbumW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetArtistW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetCommentW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetCopyrightW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetDateW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetDescriptionW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetGenreW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetISRCW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetLicenseW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetLocationW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetOrganizationW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetPerformerW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetTitleW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetTrackW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetUserItemW(string ItemKey, string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetVendorW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetVersionW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr OGGGetContactW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void OGGSetContactW(string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void SetConfigValueW(int key, int value);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void SetLogFileW(string fileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int WAVGetBitsPerSampleW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int WAVGetBlockAlignW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int WAVGetBytesPerSecondW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WAVGetCartChunkEntryW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int WAVGetChannelsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WAVGetFormatIDW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WAVGetFormatW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int WAVGetHeaderSizeW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WAVGetInfoChunkIDsW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WAVGetTextFrameW(WAVFRAMES FrameID);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WAVSaveChangesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WAVSaveChangesToFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WAVSetCartChunkEntryW(short Index, string newValue);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void WAVSetTextFrameW(WAVFRAMES FrameID, string textString);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WMAAddPictureArrayW(byte[] arr, int maxLen, string Description, short PictureType, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WMAAddPictureFileW(string FileName, string Description, short PictureType, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WMADeletePictureW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void WMADeletePicturesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WMAGetItemKeysW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int WMAGetPictureArrayW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WMAGetPictureDescriptionW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WMAGetPictureFileW(string FileName, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WMAGetPictureMimeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WMAGetPictureTypeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WMAGetPictureCountW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WMAGetUserItemW(string ItemKey);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WMAIsVBRW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WMASaveChangesToFileW(string FileName);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short WMASaveChangesW();        
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void WMASetUserItemW(string ItemKey, string textString);
				
		// new Methods with v2.0.2.0
		[LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short FLACAddPictureArrayW(byte[] arr, int maxLen, string Description, short PictureType);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short FLACAddPictureFileW(string FileName, string Description, short PictureType, short asLink);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short FLACDeletePictureW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void FLACDeletePicturesW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int FLACGetPictureArrayW(byte[] arr, int maxLen, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short FLACGetPictureCountW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]        
        private static partial IntPtr FLACGetPictureDescriptionW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short FLACGetPictureFileW(string FileName, short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr FLACGetPictureMimeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int FLACGetPictureSizeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr FLACGetPictureTypeTextW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial short FLACGetPictureTypeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int ID3V2GetPictureSizeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int MP4GetPictureSizeW(short Index);
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial int WMAGetPictureSizeW(short Index);
        // new Methods with v2.0.3.0
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial IntPtr WAVGetDisplayTextW();
        [LibraryImport("AudioGenie4.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        private static partial void WAVSetDisplayTextW(string textString);

        #endregion

        public static string[] PictureTypeText = { "Other", "File icon", "Other file icon", "Cover front", "Cover back", "Leaflet page", "media label", "Lead Artist", "Artist Performer", "Conductor", "Band Orchestra", "Composer", "Lyricist", "Recording Location", "During Recording", "During Performance", "Movie video screen capture", "A bright coloured fish", "Illustration", "Band artist logotype", "Publisher studio logotype" };


        #region APE Propertiers

        public static string APEAlbum
        {
            get { return SafeStr(APEGetAlbumW()); }
            set { APESetAlbumW(value); }
        }
        public static string APEArtist
        {
            get { return SafeStr(APEGetArtistW()); }
            set { APESetArtistW(value); }
        }
        public static string APEComment
        {
            get { return SafeStr(APEGetCommentW()); }
            set { APESetCommentW(value); }
        }
        public static string APECopyright
        {
            get { return SafeStr(APEGetCopyrightW()); }
            set { APESetCopyrightW(value); }
        }
        public static string APEGenre
        {
            get { return SafeStr(APEGetGenreW()); }
            set { APESetGenreW(value); }
        }
        public static string APETitle
        {
            get { return SafeStr(APEGetTitleW()); }
            set { APESetTitleW(value); }
        }
        public static string APETrack
        {
            get { return SafeStr(APEGetTrackW()); }
            set { APESetTrackW(value); }
        }
        public static string APEYear
        {
            get { return SafeStr(APEGetYearW()); }
            set { APESetYearW(value); }
        }

        #endregion
        #region Audio Properties

        public static string AUDIOAlbum
        {
            get { return SafeStr(AUDIOGetAlbumW()); }
            set { AUDIOSetAlbumW(value); }
        }
        public static string AUDIOArtist
        {
            get { return SafeStr(AUDIOGetArtistW()); }
            set { AUDIOSetArtistW(value); }
        }
        public static string AUDIOComment
        {
            get { return SafeStr(AUDIOGetCommentW()); }
            set { AUDIOSetCommentW(value); }
        }
        public static string AUDIOComposer
        {
            get { return SafeStr(AUDIOGetComposerW()); }
            set { AUDIOSetComposerW(value); }
        }
        public static string AUDIOGenre
        {
            get { return SafeStr(AUDIOGetGenreW()); }
            set { AUDIOSetGenreW(value); }
        }
        public static string AUDIOTitle
        {
            get { return SafeStr(AUDIOGetTitleW()); }
            set { AUDIOSetTitleW(value); }
        }
        public static string AUDIOTrack
        {
            get { return SafeStr(AUDIOGetTrackW()); }
            set { AUDIOSetTrackW(value); }
        }
        public static string AUDIOYear
        {
            get { return SafeStr(AUDIOGetYearW()); }
            set { AUDIOSetYearW(value); }
        }
        #endregion

        #region FLAC Properties
        public static string FLACAlbum
        {
            get { return SafeStr(FLACGetUserItemW("ALBUM")); }
            set { FLACSetUserItemW("ALBUM", value); }
        }
        public static string FLACArranger
        {
            get { return SafeStr(FLACGetUserItemW("ARRANGER")); }
            set { FLACSetUserItemW("ARRANGER", value); }
        }
        public static string FLACArtist
        {
            get { return SafeStr(FLACGetUserItemW("ARTIST")); }
            set { FLACSetUserItemW("ARTIST", value); }
        }
        public static string FLACAuthor
        {
            get { return SafeStr(FLACGetUserItemW("AUTHOR")); }
            set { FLACSetUserItemW("AUTHOR", value); }
        }
        public static string FLACComment
        {
            get { return SafeStr(FLACGetUserItemW("COMMENT")); }
            set { FLACSetUserItemW("COMMENT", value); }
        }
        public static string FLACComposer
        {
            get { return SafeStr(FLACGetUserItemW("COMPOSER")); }
            set { FLACSetUserItemW("COMPOSER", value); }
        }
        public static string FLACContact
        {
            get { return SafeStr(FLACGetUserItemW("CONTACT")); }
            set { FLACSetUserItemW("CONTACT", value); }
        }
        public static string FLACCopyright
        {
            get { return SafeStr(FLACGetUserItemW("COPYRIGHT")); }
            set { FLACSetUserItemW("COPYRIGHT", value); }
        }
        public static string FLACDate
        {
            get { return SafeStr(FLACGetUserItemW("DATE")); }
            set { FLACSetUserItemW("DATE", value); }
        }
        public static string FLACDescription
        {
            get { return SafeStr(FLACGetUserItemW("DESCRIPTION")); }
            set { FLACSetUserItemW("DESCRIPTION", value); }
        }
        public static string FLACDiscNumber
        {
            get { return SafeStr(FLACGetUserItemW("DISCNUMBER")); }
            set { FLACSetUserItemW("DISCNUMBER", value); }
        }
        public static string FLACEnsemble
        {
            get { return SafeStr(FLACGetUserItemW("ENSEMBLE")); }
            set { FLACSetUserItemW("ENSEMBLE", value); }
        }
        public static string FLACGenre
        {
            get { return SafeStr(FLACGetUserItemW("GENRE")); }
            set { FLACSetUserItemW("GENRE", value); }
        }
        public static string FLACISRC
        {
            get { return SafeStr(FLACGetUserItemW("ISRC")); }
            set { FLACSetUserItemW("ISRC", value); }
        }
        public static string FLACLicense
        {
            get { return SafeStr(FLACGetUserItemW("LICENSE")); }
            set { FLACSetUserItemW("LICENSE", value); }
        }
        public static string FLACLocation
        {
            get { return SafeStr(FLACGetUserItemW("LOCATION")); }
            set { FLACSetUserItemW("LOCATION", value); }
        }
        public static string FLACLyricist
        {
            get { return SafeStr(FLACGetUserItemW("LYRICIST")); }
            set { FLACSetUserItemW("LYRICIST", value); }
        }
        public static string FLACOrganization
        {
            get { return SafeStr(FLACGetUserItemW("ORGANIZATION")); }
            set { FLACSetUserItemW("ORGANIZATION", value); }
        }
        public static string FLACPerformer
        {
            get { return SafeStr(FLACGetUserItemW("PERFORMER")); }
            set { FLACSetUserItemW("PERFORMER", value); }
        }
        public static string FLACPublisher
        {
            get { return SafeStr(FLACGetUserItemW("PUBLISHER")); }
            set { FLACSetUserItemW("PUBLISHER", value); }
        }
        public static string FLACTitle
        {
            get { return SafeStr(FLACGetUserItemW("TITLE")); }
            set { FLACSetUserItemW("TITLE", value); }
        }
        public static string FLACTrack
        {
            get { return SafeStr(FLACGetUserItemW("TRACKNUMBER")); }
            set { FLACSetUserItemW("TRACKNUMBER", value); }
        }
        public static string FLACVendor
        {
            get { return SafeStr(FLACGetVendorW()); }
            set { FLACSetVendorW(value); }
        }
        public static string FLACVersion
        {
            get { return SafeStr(FLACGetUserItemW("VERSION")); }
            set { FLACSetUserItemW("VERSION", value); }
        }
        #endregion

        #region ID3V1 Properties
        public static string ID3V1Album
        {
            get { return SafeStr(ID3V1GetAlbumW()); }
            set { ID3V1SetAlbumW(value); }
        }
        public static string ID3V1Artist
        {
            get { return SafeStr(ID3V1GetArtistW()); }
            set { ID3V1SetArtistW(value); }
        }
        public static string ID3V1Comment
        {
            get { return SafeStr(ID3V1GetCommentW()); }
            set { ID3V1SetCommentW(value); }
        }
        public static short ID3V1GenreID
        {
            get { return ID3V1GetGenreIDW(); }
            set { ID3V1SetGenreIDW(value); }
        }
        public static string ID3V1Genre
        {
            get { return SafeStr(ID3V1GetGenreW()); }
            set { ID3V1SetGenreW(value); }
        }
        public static string ID3V1Title
        {
            get { return SafeStr(ID3V1GetTitleW()); }
            set { ID3V1SetTitleW(value); }
        }
        public static string ID3V1Track
        {
            get { return SafeStr(ID3V1GetTrackW()); }
            set { ID3V1SetTrackW(value); }
        }
        public static string ID3V1Year
        {
            get { return SafeStr(ID3V1GetYearW()); }
            set { ID3V1SetYearW(value); }
        }
        #endregion

        #region ID3V2 Properties
        public static string ID3V2Album
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TALB); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TALB, value); }
        }
        public static string ID3V2AlbumArtistSort
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSO2); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSO2, value); }
        }
        public static string ID3V2AlbumSort
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSOA); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSOA, value); }
        }
        public static string ID3V2Artist
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPE1); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPE1, value); }
        }
        public static string ID3V2ArtistSort
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSOP); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSOP, value); }
        }
        public static string ID3V2ArtistURL
        {
            get { return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WOAR); }
            set { ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WOAR, value); }
        }
        public static string ID3V2AudiofileURL
        {
            get { return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WOAF); }
            set { ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WOAF, value); }
        }
        public static string ID3V2AudiosourceURL
        {
            get { return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WOAS); }
            set { ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WOAS, value); }
        }
        public static string ID3V2BeatsPerMinute
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TBPM); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TBPM, value); }
        }
        public static string ID3V2Category
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TIT1); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TIT1, value); }
        }
        public static string ID3V2CommercialURL
        {
            get { return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WCOM); }
            set { ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WCOM, value); }
        }
        public static string ID3V2Composer
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TCOM); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TCOM, value); }
        }
        public static string ID3V2ComposerSort
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSOC); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSOC, value); }
        }
        public static string ID3V2ContentType
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TCON); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TCON, value); }
        }
        public static string ID3V2Conductor
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPE3); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPE3, value); }
        }
        public static string ID3V2Copyright
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TCOP); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TCOP, value); }
        }
        public static string ID3V2CopyrightURL
        {
            get { return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WCOP); }
            set { ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WCOP, value); }
        }
        public static string ID3V2Date
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDAT); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDAT, value); }
        }
        public static string ID3V2Encoder
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TENC); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TENC, value); }
        }
        public static string ID3V2EncodingSettings
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSSE); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSSE, value); }
        }
        public static string ID3V2EncodingTime
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDEN); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDEN, value); }
        }
        public static string ID3V2FileOwner
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TOWN); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TOWN, value); }
        }
        public static string ID3V2FileType
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TFLT); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TFLT, value); }
        }
        public static string ID3V2Genre
        {
            get { return SafeStr(ID3V2GetGenreW()); }
            set { ID3V2SetGenreW(value); }
        }
        public static string ID3V2InetRadioName
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TRSN); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TRSN, value); }
        }
        public static string ID3V2InetRadioOwner
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TRSO); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TRSO, value); }
        }
        public static string ID3V2InitialKey
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TKEY); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TKEY, value); }
        }
        public static string ID3V2Interpreted
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPE4); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPE4, value); }
        }
        public static string ID3V2InvolvedPeopleList
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TIPL); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TIPL, value); }
        }
        public static string ID3V2ISRC
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSRC); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSRC, value); }
        }
        public static string ID3V2Language
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TLAN); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TLAN, value); }
        }
        public static string ID3V2Length
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TLEN); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TLEN, value); }
        }
        public static string ID3V2MediaType
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TMED); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TMED, value); }
        }
        public static string ID3V2Mood
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TMOO); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TMOO, value); }
        }
        public static string ID3V2MusicianCreditsList
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TMCL); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TMCL, value); }
        }
        public static string ID3V2Orchestra
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPE2); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPE2, value); }
        }
        public static string ID3V2OriginalAlbum
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TOAL); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TOAL, value); }
        }
        public static string ID3V2OriginalArtist
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TOPE); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TOPE, value); }
        }
        public static string ID3V2OriginalFilename
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TOFN); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TOFN, value); }
        }
        public static string ID3V2OriginalReleaseTime
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDOR); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDOR, value); }
        }
        public static string ID3V2OriginalReleaseYear
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TORY); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TORY, value); }
        }
        public static string ID3V2OriginalTextwriter
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TOLY); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TOLY, value); }
        }
        public static string ID3V2PartofSet
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPOS); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPOS, value); }
        }
        public static string ID3V2PaymentURL
        {
            get { return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WPAY); }
            set { ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WPAY, value); }
        }
        public static int ID3V2PlayCounter
        {
            get { return ID3V2GetPlayCounterW(); }
            set { ID3V2SetPlayCounterW(value); }
        }
        public static string ID3V2PlaylistDelay
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDLY); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDLY, value); }
        }
        public static string ID3V2ProducedNotice
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPRO); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPRO, value); }
        }
        public static string ID3V2Publisher
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPUB); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPUB, value); }
        }
        public static string ID3V2PublisherURL
        {
            get { return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WPUB); }
            set { ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WPUB, value); }
        }
        public static string ID3V2RadiostationURL
        {
            get { return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WORS); }
            set { ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WORS, value); }
        }
        public static string ID3V2RecordingDate
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TRDA); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TRDA, value); }
        }
        public static string ID3V2RecordingTime
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDRC); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDRC, value); }
        }
        public static string ID3V2ReleaseTime
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDRL); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDRL, value); }
        }
        public static string ID3V2Size
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSIZ); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSIZ, value); }
        }
        public static string ID3V2Subtitle
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TIT3); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TIT3, value); }
        }
        public static string ID3V2SubtitleSet
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSST); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSST, value); }
        }
        public static string ID3V2TaggingTime
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDTG); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDTG, value); }
        }
        public static string ID3V2TextWriter
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TEXT); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TEXT, value); }
        }
        public static string ID3V2Time
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TIME); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TIME, value); }
        }
        public static string ID3V2Title
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TIT2); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TIT2, value); }
        }
        public static string ID3V2TitleSort
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSOT); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSOT, value); }
        }
        public static string ID3V2Track
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TRCK); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TRCK, value); }
        }
        public static string ID3V2Year
        {
            get { return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TYER); }
            set { ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TYER, value); }
        }
        #endregion

        #region LYRICS Properties
        public static string LYRICSAlbum
        {
            get { return SafeStr(LYRICSGetAlbumW()); }
            set { LYRICSSetAlbumW(value); }
        }
        public static string LYRICSArtist
        {
            get { return SafeStr(LYRICSGetArtistW()); }
            set { LYRICSSetArtistW(value); }
        }
        public static string LYRICSAuthor
        {
            get { return SafeStr(LYRICSGetAuthorW()); }
            set { LYRICSSetAuthorW(value); }
        }
        public static string LYRICSGenre
        {
            get { return SafeStr(LYRICSGetGenreW()); }
            set { LYRICSSetGenreW(value); }
        }
        public static string LYRICSImageLink
        {
            get { return SafeStr(LYRICSGetImageLinkW()); }
            set { LYRICSSetImageLinkW(value); }
        }
        public static string LYRICSIndication
        {
            get { return SafeStr(LYRICSGetIndicationW()); }
            set { LYRICSSetIndicationW(value); }
        }
        public static string LYRICSInformation
        {
            get { return SafeStr(LYRICSGetInformationW()); }
            set { LYRICSSetInformationW(value); }
        }
        public static string LYRICSLyrics
        {
            get { return SafeStr(LYRICSGetLyricsW()); }
            set { LYRICSSetLyricsW(value); }
        }
        public static string LYRICSTitle
        {
            get { return SafeStr(LYRICSGetTitleW()); }
            set { LYRICSSetTitleW(value); }
        }
        #endregion

        #region MP4 Properties
        public static string MP4Album
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_ALBUM)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_ALBUM, value); }
        }
        public static string MP4Artist
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_ARTIST)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_ARTIST, value); }
        }
        public static string MP4Author
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_AUTHOR)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_AUTHOR, value); }
        }
        public static string MP4BeatsPerMinute
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_BEATSPERMINUTE)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_BEATSPERMINUTE, value); }
        }
        public static string MP4CDSetNumber
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_CDSETNUMBER)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_CDSETNUMBER, value); }
        }
        public static string MP4Comment
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_COMMENT)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_COMMENT, value); }
        }
        public static string MP4Compilation
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_COMPILATION)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_COMPILATION, value); }
        }
        public static string MP4Composer
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_COMPOSER)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_COMPOSER, value); }
        }
        public static string MP4Copyright
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_COPYRIGHT)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_COPYRIGHT, value); }
        }
        public static string MP4Date
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_CREATEDATE)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_CREATEDATE, value); }
        }
        public static string MP4Description
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_DESCRIPTION)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_DESCRIPTION, value); }
        }
        public static string MP4Director
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_DIRECTOR)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_DIRECTOR, value); }
        }
        public static string MP4Disclaimer
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_DISCLAIMER)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_DISCLAIMER, value); }
        }
        public static string MP4Encoder
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_ENCODER)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_ENCODER, value); }
        }
        public static string MP4Genre
        {
            get { return SafeStr(MP4GetGenreW()); }
            set { MP4SetGenreW(value); }
        }
        public static string MP4NetUrl
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_NETURL)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_NETURL, value); }
        }
        public static string MP4OriginalArtist
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_ORIGINALARTIST)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_ORIGINALARTIST, value); }
        }
        public static string MP4OriginalFormat
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_ORIGINALFORMAT)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_ORIGINALFORMAT, value); }
        }
        public static string MP4OriginalSource
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_ORIGINALSOURCE)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_ORIGINALSOURCE, value); }
        }
        public static string MP4Performer
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_PERFORMER)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_PERFORMER, value); }
        }
        public static string MP4Producer
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_PRODUCER)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_PRODUCER, value); }
        }
        public static string MP4Title
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_TITLE)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_TITLE, value); }
        }
        public static string MP4Track
        {
            get { return SafeStr(MP4GetTrackW()); }
            set { MP4SetTrackW(value); }
        }
        public static string MP4Warning
        {
            get { return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_WARNING)); }
            set { MP4SetTextFrameW(MP4FRAMES.MP4_WARNING, value); }
        }
        #endregion

        #region OGG Properties
        public static string OGGAlbum
        {
            get { return SafeStr(OGGGetAlbumW()); }
            set { OGGSetAlbumW(value); }
        }
        public static string OGGArtist
        {
            get { return SafeStr(OGGGetArtistW()); }
            set { OGGSetArtistW(value); }
        }
        public static string OGGComment
        {
            get { return SafeStr(OGGGetCommentW()); }
            set { OGGSetCommentW(value); }
        }
        public static string OGGCopyright
        {
            get { return SafeStr(OGGGetCopyrightW()); }
            set { OGGSetCopyrightW(value); }
        }
        public static string OGGDate
        {
            get { return SafeStr(OGGGetDateW()); }
            set { OGGSetDateW(value); }
        }
        public static string OGGDescription
        {
            get { return SafeStr(OGGGetDescriptionW()); }
            set { OGGSetDescriptionW(value); }
        }
        public static string OGGGenre
        {
            get { return SafeStr(OGGGetGenreW()); }
            set { OGGSetGenreW(value); }
        }
        public static string OGGISRC
        {
            get { return SafeStr(OGGGetISRCW()); }
            set { OGGSetISRCW(value); }
        }
        public static string OGGLicense
        {
            get { return SafeStr(OGGGetLicenseW()); }
            set { OGGSetLicenseW(value); }
        }
        public static string OGGLocation
        {
            get { return SafeStr(OGGGetLocationW()); }
            set { OGGSetLocationW(value); }
        }
        public static string OGGOrganization
        {
            get { return SafeStr(OGGGetOrganizationW()); }
            set { OGGSetOrganizationW(value); }
        }
        public static string OGGPerformer
        {
            get { return SafeStr(OGGGetPerformerW()); }
            set { OGGSetPerformerW(value); }
        }
        public static string OGGTitle
        {
            get { return SafeStr(OGGGetTitleW()); }
            set { OGGSetTitleW(value); }
        }
        public static string OGGTrack
        {
            get { return SafeStr(OGGGetTrackW()); }
            set { OGGSetTrackW(value); }
        }
        public static string OGGVendor
        {
            get { return SafeStr(OGGGetVendorW()); }
            set { OGGSetVendorW(value); }
        }
        public static string OGGVersion
        {
            get { return SafeStr(OGGGetVersionW()); }
            set { OGGSetVersionW(value); }
        }
        public static string OGGContact
        {
            get { return SafeStr(OGGGetContactW()); }
            set { OGGSetContactW(value); }
        }
        #endregion

        #region WAV Properties
        public static string WAVArchivalLocation
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_IARL); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_IARL, value); }
        }
        public static string WAVArtist
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_IART); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_IART, value); }
        }
        public static string WAVComissioned
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ICMS); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ICMS, value); }
        }
        public static string WAVComment
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ICMT); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ICMT, value); }
        }
        public static string WAVCopyright
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ICOP); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ICOP, value); }
        }
        public static string WAVCreationDate
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ICRD); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ICRD, value); }
        }
        public static string WAVCropped
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ICRP); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ICRP, value); }
        }
        public static string WAVDimension
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_IDIM); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_IDIM, value); }
        }
        public static string WAVDisplayText
        {
            get { return SafeStr(WAVGetDisplayTextW()); }
            set { WAVSetDisplayTextW(value); }
        }
        public static string WAVDotsPerInch
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_IDPI); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_IDPI, value); }
        }
        public static string WAVEngineer
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_IENG); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_IENG, value); }
        }
        public static string WAVGenre
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_IGNR); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_IGNR, value); }
        }
        public static string WAVKeywords
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_IKEY); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_IKEY, value); }
        }
        public static string WAVLightness
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ILGT); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ILGT, value); }
        }
        public static string WAVMedium
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_IMED); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_IMED, value); }
        }
        public static string WAVName
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_INAM); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_INAM, value); }
        }
        public static string WAVPaletteSetting
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_IPLT); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_IPLT, value); }
        }
        public static string WAVProduct
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_IPRD); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_IPRD, value); }
        }
        public static string WAVSharpness
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ISHP); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ISHP, value); }
        }
        public static string WAVSoftware
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ISFT); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ISFT, value); }
        }
        public static string WAVSource
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ISRC); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ISRC, value); }
        }
        public static string WAVSourceForm
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ISRF); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ISRF, value); }
        }
        public static string WAVSubject
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ISBJ); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ISBJ, value); }
        }
        public static string WAVTechnician
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ITCH); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ITCH, value); }
        }
        public static string WAVTrack
        {
            get { return WAVGetTextFrame(WAVFRAMES.WAV_ITRK); }
            set { WAVSetTextFrame(WAVFRAMES.WAV_ITRK, value); }
        }

        public static string WAVCartVersion
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_VERSION); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_VERSION, value); }
        }

        public static string WAVCartTitle
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_TITLE); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_TITLE, value); }
        }

        public static string WAVCartArtist
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_ARTIST); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_ARTIST, value); }
        }

        public static string WAVCartCutID
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_CUTID); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_CUTID, value); }
        }

        public static string WAVCartClientID
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_CLIENTID); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_CLIENTID, value); }
        }

        public static string WAVCartCategory
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_CATEGORY); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_CATEGORY, value); }
        }

        public static string WAVCartClassification
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_CLASSIFICATION); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_CLASSIFICATION, value); }
        }

        public static string WAVCartOutCue
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_OUTCUE); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_OUTCUE, value); }
        }

        public static string WAVCartStartDate
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_STARTDATE); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_STARTDATE, value); }
        }

        public static string WAVCartStartTime
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_STARTTIME); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_STARTTIME, value); }
        }

        public static string WAVCartEndDate
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_ENDDATE); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_ENDDATE, value); }
        }

        public static string WAVCartEndTime
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_ENDTIME); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_ENDTIME, value); }
        }

        public static string WAVCartProducerAppID
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_PRODUCERAPPID); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_PRODUCERAPPID, value); }
        }

        public static string WAVCartProducerAppVersion
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_PRODUCERAPPVERSION); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_PRODUCERAPPVERSION, value); }
        }

        public static string WAVCartUserDef
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_USERDEF); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_USERDEF, value); }
        }

        public static string WAVCartLevelReference
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_LEVELREFERENCE); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_LEVELREFERENCE, value); }
        }

        public static string WAVCartPostTimer0
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER0); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER0, value); }
        }

        public static string WAVCartPostTimer1
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER1); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER1, value); }
        }

        public static string WAVCartPostTimer2
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER2); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER2, value); }
        }

        public static string WAVCartPostTimer3
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER3); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER3, value); }
        }

        public static string WAVCartPostTimer4
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER4); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER4, value); }
        }

        public static string WAVCartPostTimer5
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER5); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER5, value); }
        }

        public static string WAVCartPostTimer6
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER6); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER6, value); }
        }

        public static string WAVCartPostTimer7
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER7); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_POSTTIMER7, value); }
        }

        public static string WAVCartReserved
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_RESERVED); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_RESERVED, value); }
        }

        public static string WAVCartURL
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_URL); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_URL, value); }
        }

        public static string WAVCartTagText
        {
            get { return WAVGetCartChunkEntry(WAVCARTENTRIES.WAV_CART_TAGTEXT); }
            set { WAVSetCartChunkEntry(WAVCARTENTRIES.WAV_CART_TAGTEXT, value); }
        }
        #endregion

        #region WMA Properties
        public static string WMAAlbumArtist
        {
            get { return WMAGetUserItem(WM_ALBUMARTIST); }
            set { WMASetUserItem(WM_ALBUMARTIST, value); }
        }
        public static string WMAAlbum
        {
            get { return WMAGetUserItem(WM_ALBUMTITLE); }
            set { WMASetUserItem(WM_ALBUMTITLE, value); }
        }
        public static string WMAArtist
        {
            get { return WMAGetUserItem(WM_AUTHOR); }
            set { WMASetUserItem(WM_AUTHOR, value); }
        }
        public static string WMAAuthorURL
        {
            get { return WMAGetUserItem(WM_AUTHORURL); }
            set { WMASetUserItem(WM_AUTHORURL, value); }
        }
        public static string WMABeatsPerMinute
        {
            get { return WMAGetUserItem(WM_BEATSPERMINUTE); }
            set { WMASetUserItem(WM_BEATSPERMINUTE, value); }
        }
        public static string WMACategory
        {
            get { return WMAGetUserItem(WM_CATEGORY); }
            set { WMASetUserItem(WM_CATEGORY, value); }
        }
        public static string WMAComment
        {
            get { return WMAGetUserItem(WM_DESCRIPTION); }
            set { WMASetUserItem(WM_DESCRIPTION, value); }
        }
        public static string WMAComposer
        {
            get { return WMAGetUserItem(WM_COMPOSER); }
            set { WMASetUserItem(WM_COMPOSER, value); }
        }
        public static string WMAConductor
        {
            get { return WMAGetUserItem(WM_CONDUCTOR); }
            set { WMASetUserItem(WM_CONDUCTOR, value); }
        }
        public static string WMAContentGroupDescription
        {
            get { return WMAGetUserItem(WM_CONTENTGROUPDESCRIPTION); }
            set { WMASetUserItem(WM_CONTENTGROUPDESCRIPTION, value); }
        }
        public static string WMACopyright
        {
            get { return WMAGetUserItem(WM_PROVIDERCOPYRIGHT); }
            set { WMASetUserItem(WM_PROVIDERCOPYRIGHT, value); }
        }
        public static string WMADistributor
        {
            get { return WMAGetUserItem(WM_CONTENTDISTRIBUTOR); }
            set { WMASetUserItem(WM_CONTENTDISTRIBUTOR, value); }
        }
        public static string WMAEncodedBy
        {
            get { return WMAGetUserItem(WM_ENCODEDBY); }
            set { WMASetUserItem(WM_ENCODEDBY, value); }
        }
        public static string WMAEncodingSettings
        {
            get { return WMAGetUserItem(WM_ENCODINGSETTINGS); }
            set { WMASetUserItem(WM_ENCODINGSETTINGS, value); }
        }
        public static string WMAEncodingTime
        {
            get { return WMAGetUserItem(WM_ENCODINGTIME); }
            set { WMASetUserItem(WM_ENCODINGTIME, value); }
        }
        public static string WMAGenre
        {
            get { return WMAGetUserItem(WM_GENRE); }
            set { WMASetUserItem(WM_GENRE, value); }
        }
        public static string WMAISRC
        {
            get { return WMAGetUserItem(WM_ISRC); }
            set { WMASetUserItem(WM_ISRC, value); }
        }
        public static string WMAInitialKey
        {
            get { return WMAGetUserItem(WM_INITIALKEY); }
            set { WMASetUserItem(WM_INITIALKEY, value); }
        }
        public static string WMALanguage
        {
            get { return WMAGetUserItem(WM_LANGUAGE); }
            set { WMASetUserItem(WM_LANGUAGE, value); }
        }
        public static string WMALyrics
        {
            get { return WMAGetUserItem(WM_LYRICS); }
            set { WMASetUserItem(WM_LYRICS, value); }
        }
        public static string WMAMood
        {
            get { return WMAGetUserItem(WM_MOOD); }
            set { WMASetUserItem(WM_MOOD, value); }
        }
        public static string WMAParentalRating
        {
            get { return WMAGetUserItem(WM_PARENTALRATING); }
            set { WMASetUserItem(WM_PARENTALRATING, value); }
        }
        public static string WMAPartOfSet
        {
            get { return WMAGetUserItem(WM_PARTOFSET); }
            set { WMASetUserItem(WM_PARTOFSET, value); }
        }
        public static string WMAPeriod
        {
            get { return WMAGetUserItem(WM_PERIOD); }
            set { WMASetUserItem(WM_PERIOD, value); }
        }
        public static string WMAProducer
        {
            get { return WMAGetUserItem(WM_PRODUCER); }
            set { WMASetUserItem(WM_PRODUCER, value); }
        }
        public static string WMAPromotionURL
        {
            get { return WMAGetUserItem(WM_PROMOTIONURL); }
            set { WMASetUserItem(WM_PROMOTIONURL, value); }
        }
        public static string WMAProvider
        {
            get { return WMAGetUserItem(WM_PROVIDER); }
            set { WMASetUserItem(WM_PROVIDER, value); }
        }
        public static string WMAPublisher
        {
            get { return WMAGetUserItem(WM_PUBLISHER); }
            set { WMASetUserItem(WM_PUBLISHER, value); }
        }
        public static string WMARating
        {
            get { return WMAGetUserItem(WM_PROVIDERRATING); }
            set { WMASetUserItem(WM_PROVIDERRATING, value); }
        }
        public static string WMATitle
        {
            get { return WMAGetUserItem(WM_TITLE); }
            set { WMASetUserItem(WM_TITLE, value); }
        }
        public static string WMAToolName
        {
            get { return WMAGetUserItem(WM_TOOLNAME); }
            set { WMASetUserItem(WM_TOOLNAME, value); }
        }
        public static string WMAToolVersion
        {
            get { return WMAGetUserItem(WM_TOOLVERSION); }
            set { WMASetUserItem(WM_TOOLVERSION, value); }
        }
        public static string WMATrackNumber
        {
            get { return WMAGetUserItem(WM_TRACKNUMBER); }
            set { WMASetUserItem(WM_TRACKNUMBER, value); }
        }
        public static string WMAUserWebURL
        {
            get { return WMAGetUserItem(WM_USERWEBURL); }
            set { WMASetUserItem(WM_USERWEBURL, value); }
        }
        public static string WMAWriter
        {
            get { return WMAGetUserItem(WM_WRITER); }
            set { WMASetUserItem(WM_WRITER, value); }
        }
        public static string WMAYear
        {
            get { return WMAGetUserItem(WM_YEAR); }
            set { WMASetUserItem(WM_YEAR, value); }
        }
        #endregion

        #region AAC Methods
        public static string AACGetBitRateType()
        {
            return SafeStr(AACGetBitRateTypeW());
        }
        public static string AACGetHeaderType()
        {
            return SafeStr(AACGetHeaderTypeW());
        }
        public static string AACGetProfile()
        {
            return SafeStr(AACGetProfileW());
        }
        #endregion

        #region APE Methods
        public static bool APEExists()
        {
            return Convert.ToBoolean(APEExistsW());
        }
        public static string APEGetItemKeys()
        {
            return SafeStr(APEGetItemKeysW());
        }
        public static int APEGetSize()
        {
            return APEGetSizeW();
        }
        public static string APEGetUserItem(string Key)
        {
            return SafeStr(APEGetUserItemW(Key));
        }
        public static string APEGetVersion()
        {
            return SafeStr(APEGetVersionW());
        }
        public static bool APERemoveTagFromFile(string FileName)
        {
            return Convert.ToBoolean(APERemoveTagFromFileW(FileName));
        }
        public static bool APERemoveTag()
        {
            return Convert.ToBoolean(APERemoveTagW());
        }
        public static bool APESaveChangesToFile(string FileName)
        {
            return Convert.ToBoolean(APESaveChangesToFileW(FileName));
        }
        public static bool APESaveChanges()
        {
            return Convert.ToBoolean(APESaveChangesW());
        }
        public static void APESetUserItem(string Key, string textString)
        {
            APESetUserItemW(Key, textString);
        }
        #endregion

        #region AUDIO Methods
        public static AudioFormatID AUDIOAnalyzeFile(string FileName)
        {
            return (AudioFormatID)AUDIOAnalyzeFileW(FileName);
        }
        public static bool AUDIOIsValidFormat()
        {
            return Convert.ToBoolean(AUDIOIsValidFormatW());
        }
        public static int AUDIOGetBitrate()
        {
            return AUDIOGetBitrateW();
        }
        public static string AUDIOGetChannelMode()
        {
            return SafeStr(AUDIOGetChannelModeW());
        }
        public static int AUDIOGetChannels()
        {
            return AUDIOGetChannelsW();
        }
        public static int AUDIOGetDurationMillis()
        {
            return AUDIOGetDurationMillisW();
        }
        public static float AUDIOGetDuration()
        {
            return AUDIOGetDurationW();
        }
        public static int AUDIOGetFileSize()
        {
            return AUDIOGetFileSizeW();
        }
        public static int AUDIOGetLastErrorNumber()
        {
            return AUDIOGetLastErrorNumberW();
        }
        public static string AUDIOGetLastErrorText()
        {
            return SafeStr(AUDIOGetLastErrorTextW());
        }
        public static string AUDIOGetLastFile()
        {
            return SafeStr(AUDIOGetLastFileW());
        }
        public static string AUDIOGetMD5Value()
        {
            return SafeStr(AUDIOGetMD5ValueW());
        }
        public static int AUDIOGetSampleRate()
        {
            return AUDIOGetSampleRateW();
        }
        public static string AUDIOGetVersion()
        {
            return SafeStr(AUDIOGetVersionW());
        }
        public static bool AUDIOSaveChangesToFile(string FileName)
        {
            return Convert.ToBoolean(AUDIOSaveChangesToFileW(FileName));
        }
        public static bool AUDIOSaveChanges()
        {
            return Convert.ToBoolean(AUDIOSaveChangesW());
        }
        #endregion

        #region AudioGenie Methods
        public static int GetConfigValue(int key)
        {
            return GetConfigValueW(key);
        }
        public static string GetAudioGenieVersion()
        {
            return SafeStr(GetAudioGenieVersionW());
        }
        public static void SetConfigValue(int key, int value)
        {
            SetConfigValueW(key, value);
        }
        public static void SetLogFile(string fileName)
        {
            SetLogFileW(fileName);
        }
        public static void SetPopupKey(string keyValue)
        {
            SetPopupKeyW(keyValue);
        }
        #endregion

        #region FLAC Methods
        public static short FLACAddPictureArray(byte[] arr, string Description, short PictureType)
        {
            return FLACAddPictureArrayW(arr, arr.Length, Description, PictureType);
        }
        public static short FLACAddPictureFile(string FileName, string Description, short PictureType, short asLink)
        {
            return FLACAddPictureFileW(FileName, Description, PictureType, asLink);
        }
        public static bool FLACDeletePicture(short Index)
        {
            return Convert.ToBoolean(FLACDeletePictureW(Index));
        }
        public static void FLACDeletePictures()
        {
            FLACDeletePicturesW();
        }
        public static int FLACGetBitsPerSample()
        {
            return FLACGetBitsPerSampleW();
        }
        public static float FLACGetCompressionRatio()
        {
            return FLACGetCompressionRatioW();
        }
        public static string FLACGetItemKeys()
        {
            return SafeStr(FLACGetItemKeysW());
        }
        public static int FLACGetMaxBlockSize()
        {
            return FLACGetMaxBlockSizeW();
        }
        public static int FLACGetMaxFrameSize()
        {
            return FLACGetMaxFrameSizeW();
        }
        public static int FLACGetMinBlockSize()
        {
            return FLACGetMinBlockSizeW();
        }
        public static int FLACGetMinFrameSize()
        {
            return FLACGetMinFrameSizeW();
        }
        public static short FLACGetPictureCount()
        {
            return FLACGetPictureCountW();
        }
        public static byte[] FLACGetPictureArray(short Index)
        {
            int maxLen = (int)FLACGetPictureSize(Index);
            byte[] dummy = new byte[maxLen];
            int ln = FLACGetPictureArrayW(dummy, maxLen, Index);
            if (ln <= 0 || ln >= maxLen)
                ln = 0;
            return dummy;
        }                
        public static string FLACGetPictureDescription(short Index)
        {
            return SafeStr(FLACGetPictureDescriptionW(Index));
        }
        public static short FLACGetPictureFile(string FileName, short Index)
        {
            return FLACGetPictureFileW(FileName, Index);
        }
        public static string FLACGetPictureMime(short Index)
        {
            return SafeStr(FLACGetPictureMimeW(Index));
        }
        public static short FLACGetPictureType(short Index)
        {
            return FLACGetPictureTypeW(Index);
        }
        public static string FLACGetPictureTypeText(short Index)
        {
            return SafeStr(FLACGetPictureTypeTextW(Index));
        }
        public static int FLACGetPictureSize(short Index)
        {
            return FLACGetPictureSizeW(Index);
        }
        public static int FLACGetSamples()
        {
            return FLACGetSamplesW();
        }
        public static string FLACGetUserItem(string ItemKey)
        {
            return SafeStr(FLACGetUserItemW(ItemKey));
        }
        public static bool FLACSaveChangesToFile(string FileName)
        {
            return Convert.ToBoolean(FLACSaveChangesToFileW(FileName));
        }
        public static bool FLACSaveChanges()
        {
            return Convert.ToBoolean(FLACSaveChangesW());
        }
        public static void FLACSetUserItem(string ItemKey, string textString)
        {
            FLACSetUserItemW(ItemKey, textString);
        }
        #endregion

        public static string GetMD5ValueFromFile(string FileName)
        {
            return SafeStr(GetMD5ValueFromFileW(FileName));
        }

        #region ID3V1 Methods
        public static bool ID3V1Exists()
        {
            return Convert.ToBoolean(ID3V1ExistsW());
        }
        public static string ID3V1GetGenreItem(short number)
        {
            return SafeStr(ID3V1GetGenreItemW(number));
        }
        public static short ID3V1GetGenres()
        {
            return ID3V1GetGenresW();
        }
        public static string ID3V1GetVersion()
        {
            return SafeStr(ID3V1GetVersionW());
        }
        public static bool ID3V1RemoveTagFromFile(string FileName)
        {
            return Convert.ToBoolean(ID3V1RemoveTagFromFileW(FileName));
        }
        public static bool ID3V1RemoveTag()
        {
            return Convert.ToBoolean(ID3V1RemoveTagW());
        }
        public static bool ID3V1SaveChangesToFile(string FileName)
        {
            return Convert.ToBoolean(ID3V1SaveChangesToFileW(FileName));
        }
        public static bool ID3V1SaveChanges()
        {
            return Convert.ToBoolean(ID3V1SaveChangesW());
        }
        #endregion

        #region ID3V2 Methods
        public static short ID3V2AddAudioEncryption(byte[] arr, string URL, short PreviewStart, short PreviewLength)
        {
            return ID3V2AddAudioEncryptionW(arr, arr.Length, URL, PreviewStart, PreviewLength);
        }
        public static short ID3V2AddAudioSeekPoint(byte[] arr, int start, int length, short numbers, byte BitsPerPoint)
        {
            return ID3V2AddAudioSeekPointW(arr, arr.Length, start, length, numbers, BitsPerPoint);
        }
        public static int ID3V2GetSeekOffset()
        {
            return ID3V2GetSeekOffsetW();
        }
        public static short ID3V2AddChapter(string ID, string Title, string Description, int startTime, int endTime)
        {
            return ID3V2AddChapterW(ID, Title, Description, startTime, endTime);
        }
        public static short ID3V2AddChildElement(string ParentTocID, string ChildID)
        {
            return ID3V2AddChildElementW(ParentTocID, ChildID);
        }
        public static short ID3V2AddComment(string Language, string Description, string Text)
        {
            return ID3V2AddCommentW(Language, Description, Text);
        }
        public static short ID3V2AddCommercialFrame(byte[] arr, string Price, string validUntil, string contactUrl, short receivedAs, string seller, string description)
        {
            return ID3V2AddCommercialFrameW(arr, arr.Length, Price, validUntil, contactUrl, receivedAs, seller, description);
        }
        public static short ID3V2AddEncryption(byte[] arr, string URL, short Symbol)
        {
            return ID3V2AddEncryptionW(arr, arr.Length, URL, Symbol);
        }
        public static short ID3V2AddEqualisation(byte[] arr, byte Interpolationmethod, string Identification)
        {
            return ID3V2AddEqualisationW(arr, arr.Length, Interpolationmethod, Identification);
        }
        public static short ID3V2AddGeneralObject(byte[] arr, string Mime, string FileName, string Description)
        {
            return ID3V2AddGeneralObjectW(arr, arr.Length, Mime, FileName, Description);
        }
        public static short ID3V2AddGroupIdentification(byte[] arr, string Url, byte symbol)
        {
            return ID3V2AddGroupIdentificationW(arr, arr.Length, Url, symbol);
        }
        public static short ID3V2AddLinkedInformation(int FrameIdentifier, string URL, string additionalData)
        {
            return ID3V2AddLinkedInformationW(FrameIdentifier, URL, additionalData);
        }
        public static short ID3V2AddLyric(string Language, string Description, string Text)
        {
            return ID3V2AddLyricW(Language, Description, Text);
        }
        public static short ID3V2AddMpegLocationLookupTable(byte[] arr, int Frames, int Bytes, int Milliseconds, byte BytesDeviation, byte MillisecondsDeviation)
        {
            return ID3V2AddMpegLocationLookupTableW(arr, arr.Length, Frames, Bytes, Milliseconds, BytesDeviation, MillisecondsDeviation);
        }
        public static short ID3V2AddMusicCdIdentifier(byte[] arr)
        {
            return ID3V2AddMusicCdIdentifierW(arr, arr.Length);
        }
        public static short ID3V2AddOwnership(string Price, string dateString, string Seller)
        {
            return ID3V2AddOwnershipW(Price, dateString, Seller);
        }
        public static short ID3V2AddPictureArray(byte[] arr, string Description, short PictureType)
        {
            return ID3V2AddPictureArrayW(arr, arr.Length, Description, PictureType);
        }
        public static short ID3V2AddPictureFile(string FileName, string Description, short PictureType, short asLink)
        {
            return ID3V2AddPictureFileW(FileName, Description, PictureType, asLink);
        }
        public static short ID3V2AddPopularimeter(string Email, short Rating, int Counter)
        {
            return ID3V2AddPopularimeterW(Email, Rating, Counter);
        }
        public static short ID3V2AddPositionSynchronisation(byte TimestampFormat, int Position)
        {
            return ID3V2AddPositionSynchronisationW(TimestampFormat, Position);
        }
        public static short ID3V2AddPrivateFrame(byte[] arr, string URL)
        {
            return ID3V2AddPrivateFrameW(arr, arr.Length, URL);
        }
        public static short ID3V2AddRecommendedBufferSize(int BufferSize, byte EmbeddedInfoFlag, int Offset)
        {
            return ID3V2AddRecommendedBufferSizeW(BufferSize, EmbeddedInfoFlag, Offset);
        }
        public static short ID3V2AddRelativeVolumeAdjustment(byte[] arr, string Identifier)
        {
            return ID3V2AddRelativeVolumeAdjustmentW(arr, arr.Length, Identifier);
        }
        public static short ID3V2AddReverb(short reverbLeft, short reverbRight, byte bouncesLeft, byte bouncesRight, byte feedbackLeftToLeft, byte feedbackLeftToRight, byte feedbackRightToRight, byte feedbackRightToLeft, byte premixLeftToRight, byte premixRightToLeft)
        {
            return ID3V2AddReverbW(reverbLeft, reverbRight, bouncesLeft, bouncesRight, feedbackLeftToLeft, feedbackLeftToRight, feedbackRightToRight, feedbackRightToLeft, premixLeftToRight, premixRightToLeft);
        }
        public static short ID3V2AddSignatureFrame(byte[] arr, short GroupSymbol)
        {
            return ID3V2AddSignatureFrameW(arr, arr.Length, GroupSymbol);
        }
        public static short ID3V2AddSyncLyric(string Language, string Description, string Text, short ContentType, short TimeStampFormat)
        {
            return ID3V2AddSyncLyricW(Language, Description, Text, ContentType, TimeStampFormat);
        }
        public static short ID3V2AddSynchronizedTempo(byte[] arr, short Format)
        {
            return ID3V2AddSynchronizedTempoW(arr, arr.Length, Format);
        }
        public static short ID3V2AddTableOfContent(string ID, string Title, string Description, short isOrdered)
        {
            return ID3V2AddTableOfContentW(ID, Title, Description, isOrdered);
        }
        public static short ID3V2AddUniqueFileIdentifier(byte[] arr, string Owner)
        {
            return ID3V2AddUniqueFileIdentifierW(arr, arr.Length, Owner);
        }
        public static short ID3V2AddUserFrame(string Language, string Text)
        {
            return ID3V2AddUserFrameW(Language, Text);
        }
        public static short ID3V2AddUserText(string Description, string Text)
        {
            return ID3V2AddUserTextW(Description, Text);
        }
        public static short ID3V2AddUserURL(string Description, string URL)
        {
            return ID3V2AddUserURLW(Description, URL);
        }
        public static short ID3V2DeleteAddendum(string ID)
        {
            return ID3V2DeleteAddendumW(ID);
        }
        public static short ID3V2DeleteAllFrames(ID3V2FRAMES FrameID)
        {
            return ID3V2DeleteAllFramesW(FrameID);
        }
        public static short ID3V2DeleteChildElement(string ParentTocID, string ChildID)
        {
            return ID3V2DeleteChildElementW(ParentTocID, ChildID);
        }
        public static short ID3V2DeleteSelectedFrame(ID3V2FRAMES FrameID, short Index)
        {
            return ID3V2DeleteSelectedFrameW(FrameID, Index);
        }
        public static short ID3V2DeleteSubFrame(string ID, ID3V2FRAMES FrameID)
        {
            return ID3V2DeleteSubFrameW(ID, FrameID);
        }
        public static bool ID3V2Exists()
        {
            return Convert.ToBoolean(ID3V2ExistsW());
        }
        public static string ID3V2GetAddendumDescription(string ID)
        {
            return SafeStr(ID3V2GetAddendumDescriptionW(ID));
        }
        public static string ID3V2GetAddendumTitle(string ID)
        {
            return SafeStr(ID3V2GetAddendumTitleW(ID));
        }
        public static short ID3V2GetAddendumType(string ID)
        {
            return ID3V2GetAddendumTypeW(ID);
        }
        public static string ID3V2GetAllFrameIDs()
        {
            return SafeStr(ID3V2GetAllFrameIDsW());
        }
        public static int ID3V2GetAudioEncryptionData(byte[] arr, int maxLen, short Index)
        {
            return ID3V2GetAudioEncryptionDataW(arr, maxLen, Index);
        }
        public static short ID3V2GetAudioEncryptionPreviewLength(short Index)
        {
            return ID3V2GetAudioEncryptionPreviewLengthW(Index);
        }
        public static short ID3V2GetAudioEncryptionPreviewStart(short Index)
        {
            return ID3V2GetAudioEncryptionPreviewStartW(Index);
        }
        public static string ID3V2GetAudioEncryptionURL(short Index)
        {
            return SafeStr(ID3V2GetAudioEncryptionURLW(Index));
        }
        public static int ID3V2GetAudioSeekPointBitsPerIndexpoint()
        {
            return ID3V2GetAudioSeekPointBitsPerIndexpointW();
        }
        public static int ID3V2GetAudioSeekPointData(byte[] arr, int maxLen)
        {
            return ID3V2GetAudioSeekPointDataW(arr, maxLen);
        }
        public static int ID3V2GetAudioSeekPointLength()
        {
            return ID3V2GetAudioSeekPointLengthW();
        }
        public static int ID3V2GetAudioSeekPointNumber()
        {
            return ID3V2GetAudioSeekPointNumberW();
        }
        public static int ID3V2GetAudioSeekPointStart()
        {
            return ID3V2GetAudioSeekPointStartW();
        }
        public static int ID3v2GetChapterStartTime(string ID)
        {
            return ID3V2GetChapterStartTimeW(ID);
        }
        public static int ID3v2GetChapterEndTime(string ID)
        {
            return ID3V2GetChapterEndTimeW(ID);
        }
        public static string ID3V2GetChildElementID(string ID, short Index)
        {
            return SafeStr(ID3V2GetChildElementIDW(ID, Index));
        }
        public static short ID3V2GetChildElements(string ID)
        {
            return ID3V2GetChildElementsW(ID);
        }
        public static string ID3V2GetCommentDescription(short Index)
        {
            return SafeStr(ID3V2GetCommentDescriptionW(Index));
        }
        public static string ID3V2GetCommentLanguage(short Index)
        {
            return SafeStr(ID3V2GetCommentLanguageW(Index));
        }
        public static string ID3V2GetComment(short Index)
        {
            return SafeStr(ID3V2GetCommentW(Index));
        }
        public static string ID3V2GetCommercialFrameContactURL(short Index)
        {
            return SafeStr(ID3V2GetCommercialFrameContactURLW(Index));
        }
        public static string ID3V2GetCommercialFrameDescription(short Index)
        {
            return SafeStr(ID3V2GetCommercialFrameDescriptionW(Index));
        }
        public static string ID3V2GetCommercialFramePictureMime(short Index)
        {
            return SafeStr(ID3V2GetCommercialFramePictureMimeW(Index));
        }
        public static int ID3V2GetCommercialFramePicture(byte[] arr, int maxLen, short Index)
        {
            return ID3V2GetCommercialFramePictureW(arr, maxLen, Index);
        }
        public static string ID3V2GetCommercialFramePrice(short Index)
        {
            return SafeStr(ID3V2GetCommercialFramePriceW(Index));
        }
        public static short ID3V2GetCommercialFrameReceivedAs(short Index)
        {
            return ID3V2GetCommercialFrameReceivedAsW(Index);
        }
        public static string ID3V2GetCommercialFrameSellerName(short Index)
        {
            return SafeStr(ID3V2GetCommercialFrameSellerNameW(Index));
        }
        public static string ID3V2GetCommercialFrameValidUntil(short Index)
        {
            return SafeStr(ID3V2GetCommercialFrameValidUntilW(Index));
        }
        public static short ID3V2GetEncoding(ID3V2FRAMES FrameID)
        {
            return ID3V2GetEncodingW(FrameID);
        }
        public static int ID3V2GetEncryptionData(byte[] arr, int maxLen, short Index)
        {
            return ID3V2GetEncryptionDataW(arr, maxLen, Index);
        }
        public static short ID3V2GetEncryptionSymbol(short Index)
        {
            return ID3V2GetEncryptionSymbolW(Index);
        }
        public static string ID3V2GetEncryptionURL(short Index)
        {
            return SafeStr(ID3V2GetEncryptionURLW(Index));
        }
        public static short ID3V2GetEqualisationAdjustmentBits(short Index)
        {
            return ID3V2GetEqualisationAdjustmentBitsW(Index);
        }
        public static int ID3V2GetEqualisationData(byte[] arr, int maxLen, short Index)
        {
            return ID3V2GetEqualisationDataW(arr, maxLen, Index);
        }
        public static string ID3V2GetEqualisationIdentification(short Index)
        {
            return SafeStr(ID3V2GetEqualisationIdentificationW(Index));
        }
        public static short ID3V2GetEqualisationInterpolation(short Index)
        {
            return ID3V2GetEqualisationInterpolationW(Index);
        }
        public static int ID3V2GetEventTimingCodesData(byte[] arr, int maxLen)
        {
            return ID3V2GetEventTimingCodesDataW(arr, maxLen);
        }
        public static short ID3V2GetEventTimingCodesTimestampFormat()
        {
            return ID3V2GetEventTimingCodesTimestampFormatW();
        }
        public static short ID3V2GetFrameCount(ID3V2FRAMES FrameID)
        {
            return ID3V2GetFrameCountW(FrameID);
        }
        public static int ID3V2GetGeneralObjectData(byte[] arr, int maxLen, short Index)
        {
            return ID3V2GetGeneralObjectDataW(arr, maxLen, Index);
        }
        public static string ID3V2GetGeneralObjectDescription(short Index)
        {
            return SafeStr(ID3V2GetGeneralObjectDescriptionW(Index));
        }
        public static string ID3V2GetGeneralObjectFilename(short Index)
        {
            return SafeStr(ID3V2GetGeneralObjectFilenameW(Index));
        }
        public static string ID3V2GetGeneralObjectMime(short Index)
        {
            return SafeStr(ID3V2GetGeneralObjectMimeW(Index));
        }
        public static string ID3V2GetGenre()
        {
            return SafeStr(ID3V2GetGenreW());
        }
        public static int ID3V2GetGroupIdentificationData(byte[] arr, int maxLen, short Index)
        {
            return ID3V2GetGroupIdentificationDataW(arr, maxLen, Index);
        }
        public static short ID3V2GetGroupIdentificationSymbol(short Index)
        {
            return ID3V2GetGroupIdentificationSymbolW(Index);
        }
        public static string ID3V2GetGroupIdentificationURL(short Index)
        {
            return SafeStr(ID3V2GetGroupIdentificationURLW(Index));
        }
        public static string ID3V2GetLinkedInformationAdditionalData(short Index)
        {
            return SafeStr(ID3V2GetLinkedInformationAdditionalDataW(Index));
        }
        public static int ID3V2GetLinkedInformationFrameIdentifier(short Index)
        {
            return ID3V2GetLinkedInformationFrameIdentifierW(Index);
        }
        public static string ID3V2GetLinkedInformationURL(short Index)
        {
            return SafeStr(ID3V2GetLinkedInformationURLW(Index));
        }
        public static string ID3V2GetLyricDescription(short Index)
        {
            return SafeStr(ID3V2GetLyricDescriptionW(Index));
        }
        public static string ID3V2GetLyricLanguage(short Index)
        {
            return SafeStr(ID3V2GetLyricLanguageW(Index));
        }
        public static string ID3V2GetLyric(short Index)
        {
            return SafeStr(ID3V2GetLyricW(Index));
        }
        public static int ID3V2GetMpegLocationLookupTableBytesDeviation()
        {
            return ID3V2GetMpegLocationLookupTableBytesDeviationW();
        }
        public static int ID3V2GetMpegLocationLookupTableBytes()
        {
            return ID3V2GetMpegLocationLookupTableBytesW();
        }
        public static int ID3V2GetMpegLocationLookupTableData(byte[] arr, int maxLen)
        {
            return ID3V2GetMpegLocationLookupTableDataW(arr, maxLen);
        }
        public static int ID3V2GetMpegLocationLookupTableFrames()
        {
            return ID3V2GetMpegLocationLookupTableFramesW();
        }
        public static int ID3V2GetMpegLocationLookupTableMillisecondsDeviation()
        {
            return ID3V2GetMpegLocationLookupTableMillisecondsDeviationW();
        }
        public static int ID3V2GetMpegLocationLookupTableMilliseconds()
        {
            return ID3V2GetMpegLocationLookupTableMillisecondsW();
        }
        public static int ID3V2GetMusicCdIdentifierData(byte[] arr, int maxLen)
        {
            return ID3V2GetMusicCdIdentifierDataW(arr, maxLen);
        }
        public static string ID3V2GetOwnershipDate()
        {
            return SafeStr(ID3V2GetOwnershipDateW());
        }
        public static string ID3V2GetOwnershipPrice()
        {
            return SafeStr(ID3V2GetOwnershipPriceW());
        }
        public static string ID3V2GetOwnershipSeller()
        {
            return SafeStr(ID3V2GetOwnershipSellerW());
        }
        public static short ID3V2GetPictureCount()
        {
            return ID3V2GetFrameCount(ID3V2FRAMES.ID3F_APIC);
        }
        public static byte[] ID3V2GetPictureArray(short Index)
        {
            int maxLen = ID3V2GetPictureSize(Index);
            byte[] dummy = new byte[maxLen];
            int ln = ID3V2GetPictureArrayW(dummy, maxLen, Index);
            if (ln <= 0)
                ln = 0;
            return dummy;
        }
        public static string ID3V2GetPictureDescription(short Index)
        {
            return SafeStr(ID3V2GetPictureDescriptionW(Index));
        }
        public static short ID3V2GetPictureFile(string FileName, short Index)
        {
            return ID3V2GetPictureFileW(FileName, Index);
        }
        public static string ID3V2GetPictureMime(short Index)
        {
            return SafeStr(ID3V2GetPictureMimeW(Index));
        }
        public static short ID3V2GetPictureType(short Index)
        {
            return ID3V2GetPictureTypeW(Index);
        }
        public static string ID3V2GetPictureTypeText(short Index)
        {
            return SafeStr(ID3V2GetPictureTypeTextW(Index));
        }
        public static int ID3V2GetPictureSize(short Index)
        {
            return ID3V2GetPictureSizeW(Index);
        }
        public static int ID3V2GetPlayCounter()
        {
            return ID3V2GetPlayCounterW();
        }
        public static int ID3V2GetPopularimeterCounter(short Index)
        {
            return ID3V2GetPopularimeterCounterW(Index);
        }
        public static string ID3V2GetPopularimeterEmail(short Index)
        {
            return SafeStr(ID3V2GetPopularimeterEmailW(Index));
        }
        public static short ID3V2GetPopularimeterRating(short Index)
        {
            return ID3V2GetPopularimeterRatingW(Index);
        }
        public static short ID3V2GetPositionSynchronisationTimestampFormat()
        {
            return ID3V2GetPositionSynchronisationTimestampFormatW();
        }
        public static int ID3V2GetPositionSynchronisationValue()
        {
            return ID3V2GetPositionSynchronisationValueW();
        }
        public static string ID3V2GetPossibleCHAPID()
        {
            return SafeStr(ID3V2GetPossibleCHAPIDW());
        }
        public static string ID3V2GetPossibleTOCID()
        {
            return SafeStr(ID3V2GetPossibleTOCIDW());
        }
        public static int ID3V2GetPrivateFrameData(byte[] arr, int maxLen, short Index)
        {
            return ID3V2GetPrivateFrameDataW(arr, maxLen, Index);
        }
        public static string ID3V2GetPrivateFrameURL(short Index)
        {
            return SafeStr(ID3V2GetPrivateFrameURLW(Index));
        }
        public static int ID3V2GetRelativeVolumeAdjustmentData(byte[] arr, int maxLen, short Index)
        {
            return ID3V2GetRelativeVolumeAdjustmentDataW(arr, maxLen, Index);
        }
        public static string ID3V2GetRelativeVolumeAdjustmentIdentifier(short Index)
        {
            return SafeStr(ID3V2GetRelativeVolumeAdjustmentIdentifierW(Index));
        }
        public static short ID3V2GetReverbBouncesLeft()
        {
            return ID3V2GetReverbBouncesLeftW();
        }
        public static short ID3V2GetReverbBouncesRight()
        {
            return ID3V2GetReverbBouncesRightW();
        }
        public static short ID3V2GetReverbFeedbackLeftToLeft()
        {
            return ID3V2GetReverbFeedbackLeftToLeftW();
        }
        public static short ID3V2GetReverbFeedbackLeftToRight()
        {
            return ID3V2GetReverbFeedbackLeftToRightW();
        }
        public static short ID3V2GetReverbFeedbackRightToLeft()
        {
            return ID3V2GetReverbFeedbackRightToLeftW();
        }
        public static short ID3V2GetReverbFeedbackRightToRight()
        {
            return ID3V2GetReverbFeedbackRightToRightW();
        }
        public static short ID3V2GetReverbLeft()
        {
            return ID3V2GetReverbLeftW();
        }
        public static short ID3V2GetReverbPremixLeftToRight()
        {
            return ID3V2GetReverbPremixLeftToRightW();
        }
        public static short ID3V2GetReverbPremixRightToLeft()
        {
            return ID3V2GetReverbPremixRightToLeftW();
        }
        public static short ID3V2GetReverbRight()
        {
            return ID3V2GetReverbRightW();
        }
        public static int ID3V2GetSignatureFrameData(byte[] arr, int maxLen, short Index)
        {
            return ID3V2GetSignatureFrameDataW(arr, maxLen, Index);
        }
        public static short ID3V2GetSignatureFrameGroupSymbol(short Index)
        {
            return ID3V2GetSignatureFrameGroupSymbolW(Index);
        }
        public static int ID3V2GetSize()
        {
            return ID3V2GetSizeW();
        }
        public static string ID3V2GetSubFrameDescription(string ID, short Index)
        {
            return SafeStr(ID3V2GetSubFrameDescriptionW(ID, Index));
        }
        public static int ID3V2GetSubFrameID(string ID, short Index)
        {
            return ID3V2GetSubFrameIDW(ID, Index);
        }
        public static short ID3V2GetSubFrameImageType(string ID, short Index)
        {
            return ID3V2GetSubFrameImageTypeW(ID, Index);
        }
        public static byte[] ID3V2GetSubFrameImage(string ID, short Index)
        {
            int maxLen = 1000000;
            byte[] dummy = new byte[maxLen];
            int ln = ID3V2GetSubFrameImageW(dummy, maxLen, ID, Index);
            if (ln <= 0 || ln >= maxLen)
                ln = 0;
            Array.Resize<byte>(ref dummy, ln);
            return dummy;
        }
        public static string ID3V2GetSubFrameText(string ID, short Index)
        {
            return SafeStr(ID3V2GetSubFrameTextW(ID, Index));
        }
        public static short ID3V2GetSubFrameType(string ID, short Index)
        {
            return ID3V2GetSubFrameTypeW(ID, Index);
        }
        public static short ID3V2GetSubFrames(string ID)
        {
            return ID3V2GetSubFramesW(ID);
        }
        public static short ID3V2GetSyncLyricContentType(short Index)
        {
            return ID3V2GetSyncLyricContentTypeW(Index);
        }
        public static string ID3V2GetSyncLyricDescription(short Index)
        {
            return SafeStr(ID3V2GetSyncLyricDescriptionW(Index));
        }
        public static string ID3V2GetSyncLyricLanguage(short Index)
        {
            return SafeStr(ID3V2GetSyncLyricLanguageW(Index));
        }
        public static short ID3V2GetSyncLyricTimeFormat(short Index)
        {
            return ID3V2GetSyncLyricTimeFormatW(Index);
        }
        public static string ID3V2GetSyncLyric(short Index)
        {
            return SafeStr(ID3V2GetSyncLyricW(Index));
        }
        public static int ID3V2GetSynchronizedTempoData(byte[] arr, int maxLen)
        {
            return ID3V2GetSynchronizedTempoDataW(arr, maxLen);
        }
        public static short ID3V2GetSynchronizedTempoFormat()
        {
            return ID3V2GetSynchronizedTempoFormatW();
        }
        public static string ID3V2GetTOCRootID()
        {
            return SafeStr(ID3V2GetTOCRootIDW());
        }
        public static string ID3V2GetTextFrame(ID3V2FRAMES FrameID)
        {
            return SafeStr(ID3V2GetTextFrameW(FrameID));
        }
        public static string ID3V2GetURLFrame(ID3V2FRAMES FrameID)
        {
            return SafeStr(ID3V2GetURLFrameW(FrameID));
        }
        public static int ID3V2GetUniqueFileIdentifierData(byte[] arr, int maxLen, short Index)
        {
            return ID3V2GetUniqueFileIdentifierDataW(arr, maxLen, Index);
        }
        public static string ID3V2GetUniqueFileIdentifierOwner(short Index)
        {
            return SafeStr(ID3V2GetUniqueFileIdentifierOwnerW(Index));
        }
        public static string ID3V2GetUserFrameLanguage(short Index)
        {
            return SafeStr(ID3V2GetUserFrameLanguageW(Index));
        }
        public static string ID3V2GetUserFrame(short Index)
        {
            return SafeStr(ID3V2GetUserFrameW(Index));
        }
        public static string ID3V2GetUserTextDescription(short Index)
        {
            return SafeStr(ID3V2GetUserTextDescriptionW(Index));
        }
        public static string ID3V2GetUserText(short Index)
        {
            return SafeStr(ID3V2GetUserTextW(Index));
        }
        public static string ID3V2GetUserURLDescription(short Index)
        {
            return SafeStr(ID3V2GetUserURLDescriptionW(Index));
        }
        public static string ID3V2GetUserURL(short Index)
        {
            return SafeStr(ID3V2GetUserURLW(Index));
        }
        public static string ID3V2GetVersion()
        {
            return SafeStr(ID3V2GetVersionW());
        }
        public static bool ID3V2GetTOCIsOrdered(string ID)
        {
            return Convert.ToBoolean(ID3V2GetTOCIsOrderedW(ID));
        }
        public static short ID3V2GetRecommendedBufferSizeFlag()
        {
            return ID3V2GetRecommendedBufferSizeFlagW();
        }
        public static int ID3V2RecommendedBufferSizeOffset()
        {
            return ID3V2GetRecommendedBufferSizeOffsetW();
        }
        public static int ID3V2GetRecommendedBufferSizeValue()
        {
            return ID3V2GetRecommendedBufferSizeValueW();
        }
        public static short ID3V2RemoveTagFromFile(string FileName)
        {
            return ID3V2RemoveTagFromFileW(FileName);
        }
        public static short ID3V2RemoveTag()
        {
            return ID3V2RemoveTagW();
        }
        public static short ID3V2AddEventTimingCodes(byte[] arr, int maxLen, byte TimestampFormat)
        {
            return ID3V2AddEventTimingCodesW(arr, maxLen, TimestampFormat);
        }
        public static short ID3V2SaveChangesToFile(string FileName)
        {
            return ID3V2SaveChangesToFileW(FileName);
        }
        public static short ID3V2SaveChanges()
        {
            return ID3V2SaveChangesW();
        }
        public static int ID3v2SetAddendumDescription(string ID, string newDescription)
        {
            return ID3V2SetAddendumDescriptionW(ID, newDescription);
        }
        public static int ID3v2SetAddendumTitle(string ID, string newTitle)
        {
            return ID3V2SetAddendumTitleW(ID, newTitle);
        }
        public static void ID3V2SetChapterTimes(string ID, int startTime, int endTime)
        {
            ID3V2SetChapterTimesW(ID, startTime, endTime);
        }
        public static short ID3V2SetFormatAndEncoding(short format, short encoding)
        {
            return ID3V2SetFormatAndEncodingW(format, encoding);
        }
        public static void ID3V2SetGenre(string textString)
        {
            ID3V2SetGenreW(textString);
        }
        public static short ID3V2SetImageSubFrame(byte[] arr, string ID, string Description, PictureType pictureType)
        {
            return ID3V2SetImageSubFrameW(arr, arr.Length, ID, Description, pictureType);
        }
        public static void ID3V2SetPlayCounter(int counter)
        {
            ID3V2SetPlayCounterW(counter);
        }
        public static void ID3V2AddSeekOffset(int offset)
        {
            ID3V2AddSeekOffsetW(offset);
        }
        public static void ID3V2SetTextFrame(ID3V2FRAMES FrameID, string textString)
        {
            ID3V2SetTextFrameW(FrameID, textString);
        }
        public static short ID3V2SetTextSubFrame(string ID, ID3V2FRAMES FrameID, string textString, string description)
        {
            return ID3V2SetTextSubFrameW(ID, FrameID, textString, description);
        }
        public static void ID3V2SetURLFrame(ID3V2FRAMES FrameID, string textString)
        {
            ID3V2SetURLFrameW(FrameID, textString);
        }
        public static short ID3V2SetURLSubFrame(string ID, ID3V2FRAMES FrameID, string urlString, string description)
        {
            return ID3V2SetURLSubFrameW(ID, FrameID, urlString, description);
        }
        public static short ID3V2SetTOCIsOrdered(string ID, bool isOrdered)
        {
            return ID3V2SetTOCIsOrderedW(ID, Convert.ToInt16(isOrdered));
        }
        public static short ID3V2ImportCueFile(string FileName)
        {
            return ID3V2ImportCueFileW(FileName);
        }
        #endregion

        #region LYRICS Methods
        public static bool LYRICSExists()
        {
            return Convert.ToBoolean(LYRICSExistsW());
        }
        public static int LYRICSGetSize()
        {
            return LYRICSGetSizeW();
        }
        public static int LYRICSGetStartPosition()
        {
            return LYRICSGetStartPositionW();
        }
        public static string LYRICSGetVersion()
        {
            return SafeStr(LYRICSGetVersionW());
        }
        public static bool LYRICSRemoveTagFromFile(string FileName)
        {
            return Convert.ToBoolean(LYRICSRemoveTagFromFileW(FileName));
        }
        public static bool LYRICSRemoveTag()
        {
            return Convert.ToBoolean(LYRICSRemoveTagW());
        }
        public static bool LYRICSSaveChangesToFile(string FileName)
        {
            return Convert.ToBoolean(LYRICSSaveChangesToFileW(FileName));
        }
        public static bool LYRICSSaveChanges()
        {
            return Convert.ToBoolean(LYRICSSaveChangesW());
        }
        #endregion

        #region MONKEY Methods
        public static short MONKEYGetBits()
        {
            return MONKEYGetBitsW();
        }
        public static float MONKEYGetCompressionRatio()
        {
            return MONKEYGetCompressionRatioW();
        }
        public static string MONKEYGetCompression()
        {
            return SafeStr(MONKEYGetCompressionW());
        }
        public static int MONKEYGetFrames()
        {
            return MONKEYGetFramesW();
        }
        public static float MONKEYGetPeak()
        {
            return MONKEYGetPeakW();
        }
        public static int MONKEYGetSamplesPerFrame()
        {
            return MONKEYGetSamplesPerFrameW();
        }
        public static int MONKEYGetSamples()
        {
            return MONKEYGetSamplesW();
        }
        #endregion

        #region MP4 Methods
        public static void MP4DeleteEntries()
        {
            MP4DeleteEntriesW();
        }
        public static void MP4DeletePicture(short Index)
        {
            MP4DeletePictureW(Index);
        }
        public static void MP4DeletePictures()
        {
            MP4DeletePicturesW();
        }
        public static string MP4GetAllFrameIDs()
        {
            return SafeStr(MP4GetAllFrameIDsW());
        }
        public static string MP4GetiTuneFrame(string FrameID)
        {
            return SafeStr(MP4GetiTuneFrameW(FrameID));
        }
        public static string MP4GetTextFrame(MP4FRAMES FrameID)
        {
            return SafeStr(MP4GetTextFrameW(FrameID));
        }
        public static byte[] MP4GetPictureArray(short Index)
        {
            int maxLen = MP4GetPictureSize(Index);
            byte[] dummy = new byte[maxLen];
            int ln = MP4GetPictureArrayW(dummy, maxLen, Index);
            if (ln <= 0)
                ln = 0;
            return dummy;
        }
        public static int MP4GetPictureCount()
        {
            return MP4GetPictureCountW();
        }
        public static short MP4GetPictureFile(string FileName, short Index)
        {
            return MP4GetPictureFileW(FileName, Index);
        }
        public static string MP4GetPictureMime(short Index)
        {
            return SafeStr(MP4GetPictureMimeW(Index));
        }
        public static int MP4GetPictureSize(short Index)
        {
            return MP4GetPictureSizeW(Index);
        }
        public static string MP4GetVersion()
        {
            return SafeStr(MP4GetVersionW());
        }
        public static bool MP4SaveChangesToFile(string FileName)
        {
            return Convert.ToBoolean(MP4SaveChangesToFileW(FileName));
        }
        public static bool MP4SaveChanges()
        {
            return Convert.ToBoolean(MP4SaveChangesW());
        }
        public static short MP4AddPictureArray(byte[] arr)
        {
            return MP4AddPictureArrayW(arr, arr.Length);
        }
        public static short MP4AddPictureFile(string FileName)
        {
            return MP4AddPictureFileW(FileName);
        }
        public static void MP4SetiTuneFrame(string FrameID, string textString)
        {
            MP4SetiTuneFrameW(FrameID, textString);
        }
        public static void MP4SetTextFrame(MP4FRAMES FrameID, string textString)
        {
            MP4SetTextFrameW(FrameID, textString);
        }
        #endregion

        #region MPEG Methods
        public static string MPEGGetEmphasis()
        {
            return SafeStr(MPEGGetEmphasisW());
        }
        public static string MPEGGetEncoder()
        {
            return SafeStr(MPEGGetEncoderW());
        }
        public static int MPEGGetFramePosition()
        {
            return MPEGGetFramePositionW();
        }
        public static int MPEGGetFrameSize()
        {
            return MPEGGetFrameSizeW();
        }
        public static int MPEGGetFrames()
        {
            return MPEGGetFramesW();
        }
        public static string MPEGGetLayer()
        {
            return SafeStr(MPEGGetLayerW());
        }
        public static string MPEGGetVersion()
        {
            return SafeStr(MPEGGetVersionW());
        }
        public static bool MPEGGetIsCopyrighted()
        {
            return Convert.ToBoolean(MPEGIsCopyrightedW());
        }
        public static bool MPEGIsOriginal()
        {
            return Convert.ToBoolean(MPEGIsOriginalW());
        }
        public static bool MPEGIsPadding()
        {
            return Convert.ToBoolean(MPEGIsPaddingW());
        }
        public static bool MPEGIsPrivate()
        {
            return Convert.ToBoolean(MPEGIsPrivateW());
        }
        public static bool MPEGIsProtected()
        {
            return Convert.ToBoolean(MPEGIsProtectedW());
        }
        public static bool MPEGIsVBR()
        {
            return Convert.ToBoolean(MPEGIsVBRW());
        }
        public static bool MPEGSetCopyrighted(string FileName, short newValue)
        {
            return Convert.ToBoolean(MPEGSetCopyrightedW(FileName, newValue));
        }
        public static bool MPEGSetOriginal(string FileName, short newValue)
        {
            return Convert.ToBoolean(MPEGSetOriginalW(FileName, newValue));
        }
        public static bool MPEGSetPrivate(string FileName, short newValue)
        {
            return Convert.ToBoolean(MPEGSetPrivateW(FileName, newValue));
        }
        #endregion

        #region MPP Methods
        public static int MPPGetFrames()
        {
            return MPPGetFramesW();
        }
        public static short MPPGetStreamVersion()
        {
            return MPPGetStreamVersionW();
        }
        #endregion

        #region OGG Methods
        public static int OGGGetBitRateNominal()
        {
            return OGGGetBitRateNominalW();
        }
        public static string OGGGetItemKeys()
        {
            return SafeStr(OGGGetItemKeysW());
        }
        public static string OGGGetUserItem(string ItemKey)
        {
            return SafeStr(OGGGetUserItemW(ItemKey));
        }
        public static short OGGRemoveTagFromFile(string FileName)
        {
            return OGGRemoveTagFromFileW(FileName);
        }
        public static bool OGGRemoveTag()
        {
            return Convert.ToBoolean(OGGRemoveTagW());
        }
        public static bool OGGSaveChangesToFile(string FileName)
        {
            return Convert.ToBoolean(OGGSaveChangesToFileW(FileName));
        }
        public static bool OGGSaveChanges()
        {
            return Convert.ToBoolean(OGGSaveChangesW());
        }
        public static void OGGSetUserItem(string ItemKey, string textString)
        {
            OGGSetUserItemW(ItemKey, textString);
        }
        #endregion

        #region WAV Methods
        public static int WAVGetBitsPerSample()
        {
            return WAVGetBitsPerSampleW();
        }
        public static int WAVGetBlockAlign()
        {
            return WAVGetBlockAlignW();
        }
        public static int WAVGetBytesPerSecond()
        {
            return WAVGetBytesPerSecondW();
        }
        public static int WAVGetChannels()
        {
            return WAVGetChannelsW();
        }
        public static short WAVGetFormatID()
        {
            return WAVGetFormatIDW();
        }
        public static string WAVGetFormat()
        {
            return SafeStr(WAVGetFormatW());
        }
        public static int WAVGetHeaderSize()
        {
            return WAVGetHeaderSizeW();
        }
        public static string WAVGetInfoChunkIDs()
        {
            return SafeStr(WAVGetInfoChunkIDsW());
        }
        public static string WAVGetTextFrame(WAVFRAMES FrameID)
        {
            return SafeStr(WAVGetTextFrameW(FrameID));
        }
        public static void WAVSetTextFrame(WAVFRAMES FrameID, string textString)
        {
            WAVSetTextFrameW(FrameID, textString);
        }
        public static string WAVGetCartChunkEntry(WAVCARTENTRIES Index)
        {
            return SafeStr(WAVGetCartChunkEntryW((short)Index));
        }
        public static void WAVSetCartChunkEntry(WAVCARTENTRIES Index, string textString)
        {
            WAVSetCartChunkEntryW((short)Index, textString);
        }
        public static bool WAVSaveChanges()
        {
            return Convert.ToBoolean(WAVSaveChangesW());
        }
        public static bool WAVSaveChangesToFile(string FileName)
        {
            return Convert.ToBoolean(WAVSaveChangesToFileW(FileName));
        }
        
        #endregion
        #region WMA Methods
        public static bool WMADeletePicture(short Index)
        {
            return Convert.ToBoolean(WMADeletePictureW(Index));
        }
        public static void WMADeletePictures()
        {
            WMADeletePicturesW();
        }
        public static string WMAGetItemKeys()
        {
            return SafeStr(WMAGetItemKeysW());
        }
        public static byte[] WMAGetPictureArray(short Index)
        {
            int maxLen = (int)WMAGetPictureSize(Index);
            byte[] dummy = new byte[maxLen];
            int ln = WMAGetPictureArrayW(dummy, maxLen, Index);
            if (ln <= 0)
                ln = 0;
            return dummy;
        }
        public static string WMAGetPictureDescription(short Index)
        {
            return SafeStr(WMAGetPictureDescriptionW(Index));
        }
        public static string WMAGetPictureMime(short Index)
        {
            return SafeStr(WMAGetPictureMimeW(Index));
        }
        public static int WMAGetPictureSize(short Index)
        {
            return WMAGetPictureSizeW(Index);
        }
        public static short WMAGetPictureType(short Index)
        {
            return WMAGetPictureTypeW(Index);
        }
        public static short WMAGetPictureFile(string FileName, short Index)
        {
            return WMAGetPictureFileW(FileName, Index);
        }
        public static short WMAGetPictureCount()
        {
            return WMAGetPictureCountW();
        }
        public static string WMAGetUserItem(string ItemKey)
        {
            return SafeStr(WMAGetUserItemW(ItemKey));
        }
        public static bool WMAIsVBR()
        {
            return Convert.ToBoolean(WMAIsVBRW());
        }
        public static bool WMASaveChangesToFile(string FileName)
        {
            return Convert.ToBoolean(WMASaveChangesToFileW(FileName));
        }
        public static bool WMASaveChanges()
        {
            return Convert.ToBoolean(WMASaveChangesW());
        }
        public static short WMAAddPictureArray(byte[] arr, string Description, short PictureType, short Index)
        {
            return WMAAddPictureArrayW(arr, arr.Length, Description, PictureType, Index);
        }
        public static short WMAAddPictureFile(string FileName, string Description, short PictureType, short Index)
        {
            return WMAAddPictureFileW(FileName, Description, PictureType, Index);
        }
        public static void WMASetUserItem(string ItemKey, string textString)
        {
            WMASetUserItemW(ItemKey, textString);
        }
        #endregion

    }
}


