' *******************************************************
' ************ AUDIOGENIE DLL WRAPPER FOR VB ************
' *******************************************************
' * This is a Wrapper for the AudioGenie4.dll           *
' * written by Stefan Toengi.                           *
' * Many thanks to Tobias Lindner for the original file *
' * Refactored for .Net Core 10 and use in RapidTagger  *
' * by William Harvey 2026.                             *
' *                                                     *
' * To use this class, copy this file in your project   *
' * and the AudioGenie4.dll in the %Windows%\%system32% *
' * (32bit OS) or %Windows%\SysWOW64 folder(64bit OS)   *
' * or in the application folder. Now you can use this  *
' * class like the example:                             *
' *                                                     *
' * Imports AudioGenie                                  *
' * AudioGenie4.SetPopupKey(keyValue)                   *
' * AudioGenie4.AUDIOAnalyzeFile("test.mp3")            *
' * AudioGenie4.AUDIOTitle = "TEST"                     *
' * AudioGenie4.AUDIOSaveChanges()                      *

Option Explicit On
Option Strict On

Imports System
Imports System.Text
Imports System.Runtime.InteropServices

Namespace AudioGenie

#Region "Public Enums"

    Public Enum AudioFormatID As Short
        UNKNOWN = 0
        MPEG = 1
        WMA = 2
        MONKEY = 3
        FLAC = 4
        WAV = 5
        OGGVORBIS = 6
        MPEGPLUS = 7
        AAC = 8
        MP4M4A = 9
        TTA = 10
        WAVPACK = 11
    End Enum

    Public Enum PictureType As Short
        OTHER = 0
        FILE_ICON = 1
        OTHER_FILE_ICON = 2
        COVER_FRONT = 3
        COVER_BACK = 4
        LEAFLET_PAGE = 5
        MEDIA_LABEL = 6
        LEAD_ARTIST = 7
        ARTIST_PERFORMER = 8
        CONDUCTOR = 9
        BAND_ORCHESTRA = 10
        COMPOSER = 11
        LYRICIST = 12
        RECORDING_LOCATION = 13
        DURING_RECORDING = 14
        DURING_PERFORMANCE = 15
        MOVIE_VIDEO_SCREEN_CAPTURE = 16
        A_BRIGHT_COLOURED_FISH = 17
        ILLUSTRATION = 18
        BAND_ARTIST_LOGOTYPE = 19
        PUBLISHER_STUDIO_LOGOTYPE = 20
    End Enum

    Public Enum PictureLink As Short
        AS_FILENAME = -1
        AS_PICTURE = 0
    End Enum

    Public Enum ID3V2FRAMES As UInteger
        ID3F_AENC = &H41454E43UI
        ID3F_APIC = &H41504943UI
        ID3F_ASPI = &H41535049UI
        ID3F_CHAP = &H43484150UI
        ID3F_COMM = &H434F4D4DUI
        ID3F_COMR = &H434F4D52UI
        ID3F_CTOC = &H43544F43UI
        ID3F_ENCR = &H454E4352UI
        ID3F_EQU2 = &H45515532UI
        ID3F_EQUA = &H45515541UI
        ID3F_ETCO = &H4554434FUI
        ID3F_GEOB = &H47454F42UI
        ID3F_GRID = &H47524944UI
        ID3F_IPLS = &H49504C53UI
        ID3F_LINK = &H4C494E4BUI
        ID3F_MCDI = &H4D434449UI
        ID3F_MLLT = &H4D4C4C54UI
        ID3F_OWNE = &H4F574E45UI
        ID3F_PCNT = &H50434E54UI
        ID3F_POPM = &H504F504DUI
        ID3F_POSS = &H504F5353UI
        ID3F_PRIV = &H50524956UI
        ID3F_RBUF = &H52425546UI
        ID3F_RVA2 = &H52564132UI
        ID3F_RVAD = &H52564144UI
        ID3F_RVRB = &H52565242UI
        ID3F_SEEK = &H5345454BUI
        ID3F_SIGN = &H5349474EUI
        ID3F_SYLT = &H53594C54UI
        ID3F_SYTC = &H53595443UI
        ID3F_TALB = &H54414C42UI
        ID3F_TBPM = &H5442504DUI
        ID3F_TCMP = &H54434D50UI  'itunes
        ID3F_TCOM = &H54434F4DUI
        ID3F_TCON = &H54434F4EUI
        ID3F_TCOP = &H54434F50UI
        ID3F_TDAT = &H54444154UI
        ID3F_TDEN = &H5444454EUI
        ID3F_TDLY = &H54444C59UI
        ID3F_TDOR = &H54444F52UI
        ID3F_TDRC = &H54445243UI
        ID3F_TDRL = &H5444524CUI
        ID3F_TDTG = &H54445447UI
        ID3F_TENC = &H54454E43UI
        ID3F_TEXT = &H54455854UI
        ID3F_TFLT = &H54464C54UI
        ID3F_TIME = &H54494D45UI
        ID3F_TIPL = &H5449504CUI
        ID3F_TIT1 = &H54495431UI
        ID3F_TIT2 = &H54495432UI
        ID3F_TIT3 = &H54495433UI
        ID3F_TKEY = &H544B4559UI
        ID3F_TLAN = &H544C414EUI
        ID3F_TLEN = &H544C454EUI
        ID3F_TMCL = &H544D434CUI
        ID3F_TMED = &H544D4544UI
        ID3F_TMOO = &H544D4F4FUI
        ID3F_TOAL = &H544F414CUI
        ID3F_TOFN = &H544F464EUI
        ID3F_TOLY = &H544F4C59UI
        ID3F_TOPE = &H544F5045UI
        ID3F_TORY = &H544F5259UI
        ID3F_TOWN = &H544F574EUI
        ID3F_TPE1 = &H54504531UI
        ID3F_TPE2 = &H54504532UI
        ID3F_TPE3 = &H54504533UI
        ID3F_TPE4 = &H54504534UI
        ID3F_TPOS = &H54504F53UI
        ID3F_TPRO = &H5450524FUI
        ID3F_TPUB = &H54505542UI
        ID3F_TRCK = &H5452434BUI
        ID3F_TRDA = &H54524441UI
        ID3F_TRSN = &H5452534EUI
        ID3F_TRSO = &H5452534FUI
        ID3F_TSIZ = &H5453495AUI
        ID3F_TSO2 = &H54534F32UI 'itunes
        ID3F_TSOA = &H54534F41UI 'itunes
        ID3F_TSOC = &H54534F43UI 'itunes
        ID3F_TSOP = &H54534F50UI 'itunes
        ID3F_TSOT = &H54534F54UI 'itunes
        ID3F_TSRC = &H54535243UI
        ID3F_TSSE = &H54535345UI
        ID3F_TSST = &H54535354UI
        ID3F_TXXX = &H54585858UI
        ID3F_TYER = &H54594552UI
        ID3F_UFID = &H55464944UI
        ID3F_USER = &H55534552UI
        ID3F_USLT = &H55534C54UI
        ID3F_WCOM = &H57434F4DUI
        ID3F_WCOP = &H57434F50UI
        ID3F_WOAF = &H574F4146UI
        ID3F_WOAR = &H574F4152UI
        ID3F_WOAS = &H574F4153UI
        ID3F_WORS = &H574F5253UI
        ID3F_WPAY = &H57504159UI
        ID3F_WPUB = &H57505542UI
        ID3F_WXXX = &H57585858UI
    End Enum

    Public Enum MP4FRAMES As UInteger
        MP4_ALBUM = &HA9616C62UI          ' Â©alb
        MP4_ARTIST = &HA9415254UI         ' Â©ART
        MP4_AUTHOR = &HA9617574UI         ' Â©aut
        MP4_ALBUMARTIST = &H61415254UI    ' aART
        MP4_BEATSPERMINUTE = &H746D706FUI ' tmpo
        MP4_CATEGORY = &H63617467UI       ' catg
        MP4_CDSETNUMBER = &H2064736BUI    '  dsk
        MP4_DISCNUMBER = &H6469736BUI     ' disk
        MP4_COMMENT = &HA9636D74UI        ' Â©cmt
        MP4_COMPOSER = &HA9777274UI       ' Â©wrt
        MP4_COMPILATION = &H6370696CUI    ' cpil
        MP4_DAY = &HA9646179UI            ' Â©day
        MP4_COPYRIGHT = &HA9637079UI      ' Â©cpy
        MP4_CREATEDATE = &HA9646179UI     ' Â©day
        MP4_DESCRIPTION = &HA9646573UI    ' Â©des
        MP4_INFORMATION = &HA9696E66UI    ' Â©inf
        MP4_DIRECTOR = &HA9646972UI       ' Â©dir
        MP4_DISCLAIMER = &HA9646973UI     ' Â©dis
        MP4_ENCODER = &HA9746F6FUI        ' Â©too
        MP4_GROUPING = &HA9677270UI       ' Â©grp
        MP4_KEYWORD = &H6B657977UI        ' keyw
        MP4_LYRICS = &HA96C7972UI         ' Â©lyr
        MP4_TITLE = &HA96E616DUI          ' Â©nam
        MP4_NETURL = &HA975726CUI         ' Â©url
        MP4_ORIGINALARTIST = &HA96F7065UI ' Â©ope
        MP4_ORIGINALFORMAT = &HA9666D74UI ' Â©fmt
        MP4_ORIGINALSOURCE = &HA9737263UI ' Â©src
        MP4_PERFORMER = &HA9707266UI      ' Â©prf
        MP4_PRODUCER = &HA9707264UI       ' Â©prd
        MP4_RATING = &H72617465UI         ' rate
        MP4_WARNING = &HA977726EUI        ' Â©wrn
    End Enum

    Public Enum WAVFRAMES As UInteger
        WAV_IARL = &H4941524CUI ' ArchivalLocation
        WAV_IART = &H49415254UI ' Artist
        WAV_ICMS = &H49434D53UI ' Comissioned
        WAV_ICMT = &H49434D54UI ' Comment
        WAV_ICOP = &H49434F50UI ' Copyright
        WAV_ICRD = &H49435244UI ' CreationDate
        WAV_ICRP = &H49435250UI ' Cropped
        WAV_IDIM = &H4944494DUI ' Dimension
        WAV_IDPI = &H49445049UI ' DotsPerInch
        WAV_IENG = &H49454E47UI ' Engineer
        WAV_IGNR = &H49474E52UI ' Genre
        WAV_IKEY = &H494B4559UI ' Keywords
        WAV_ILGT = &H494C4754UI ' Lightness
        WAV_IMED = &H494D4544UI ' Medium
        WAV_INAM = &H494E414DUI ' Name
        WAV_IPLT = &H49504C54UI ' PaletteSetting
        WAV_IPRD = &H49505244UI ' Product
        WAV_IRTD = &H49525444UI ' Rating
        WAV_ISBJ = &H4953424AUI ' Subject
        WAV_ISFT = &H49534654UI ' Software
        WAV_ISHP = &H49534850UI ' Sharpness
        WAV_ISRC = &H49535243UI ' Source
        WAV_ISRF = &H49535246UI ' SourceForm
        WAV_ITCH = &H49544348UI ' Technician
        WAV_ITRK = &H4954524BUI ' Track
    End Enum

    Public Enum WAVCARTENTRIES As Short
        WAV_CART_VERSION = 0
        WAV_CART_TITLE = 1
        WAV_CART_ARTIST = 2
        WAV_CART_CUTID = 3
        WAV_CART_CLIENTID = 4
        WAV_CART_CATEGORY = 5
        WAV_CART_CLASSIFICATION = 6
        WAV_CART_OUTCUE = 7
        WAV_CART_STARTDATE = 8
        WAV_CART_STARTTIME = 9
        WAV_CART_ENDDATE = 10
        WAV_CART_ENDTIME = 11
        WAV_CART_PRODUCERAPPID = 12
        WAV_CART_PRODUCERAPPVERSION = 13
        WAV_CART_USERDEF = 14
        WAV_CART_LEVELREFERENCE = 15
        WAV_CART_POSTTIMER0 = 16
        WAV_CART_POSTTIMER1 = 17
        WAV_CART_POSTTIMER2 = 18
        WAV_CART_POSTTIMER3 = 19
        WAV_CART_POSTTIMER4 = 20
        WAV_CART_POSTTIMER5 = 21
        WAV_CART_POSTTIMER6 = 22
        WAV_CART_POSTTIMER7 = 23
        WAV_CART_RESERVED = 24
        WAV_CART_URL = 25
        WAV_CART_TAGTEXT = 26
    End Enum

    Public Enum SyncLyricContentTypes As Short
        UNGUELTIG = -1
        SONSTIGES = 0
        SONGTEXTE = 1
        TEXTUMSCHREIBUNGEN = 2
        BEWEGUNGEN = 3
        EREIGNISSE = 4
        AKKORDE = 5
        BELANGLOSES = 6
        URLS_ZU_WEBSEITEN = 7
        URLS_ZU_BILDERN = 8
    End Enum

    Public Enum SyncLyricTimeFormat As Short
        UNGUELTIG = -1
        MPEGFRAMES = 1
        MILLISEKUNDEN = 2
    End Enum

#End Region

    Public NotInheritable Class AudioGenie4

        Private Sub New()
        End Sub

        Private Shared Function SafeStr(ptr As IntPtr) As String
            If ptr = IntPtr.Zero Then Return String.Empty
            Dim result As String = Marshal.PtrToStringUni(ptr)
            If result Is Nothing Then Return String.Empty
            Return result
        End Function

        Public Shared ReadOnly PictureTypeText() As String = {
            "Other", "File icon", "Other file icon", "Cover front", "Cover back",
            "Leaflet page", "media label", "Lead Artist", "Artist Performer",
            "Conductor", "Band Orchestra", "Composer", "Lyricist",
            "Recording Location", "During Recording", "During Performance",
            "Movie video screen capture", "A bright coloured fish", "Illustration",
            "Band artist logotype", "Publisher studio logotype"}

#Region "WM Constants"
        Public Const WM_ALBUMARTIST As String = "WM/AlbumArtist"
        Public Const WM_ALBUMTITLE As String = "WM/AlbumTitle"
        Public Const WM_AUTHOR As String = "WM/Author"
        Public Const WM_AUTHORURL As String = "WM/AuthorURL"
        Public Const WM_BEATSPERMINUTE As String = "WM/BeatsPerMinute"
        Public Const WM_CATEGORY As String = "WM/Category"
        Public Const WM_COMPOSER As String = "WM/Composer"
        Public Const WM_CONDUCTOR As String = "WM/Conductor"
        Public Const WM_CONTENTDISTRIBUTOR As String = "WM/ContentDistributor"
        Public Const WM_CONTENTGROUPDESCRIPTION As String = "WM/ContentGroupDescription"
        Public Const WM_DESCRIPTION As String = "WM/Description"
        Public Const WM_DISCNUMBER As String = "WM/PartOfSet"
        Public Const WM_ENCODEDBY As String = "WM/EncodedBy"
        Public Const WM_ENCODINGSETTINGS As String = "WM/EncodingSettings"
        Public Const WM_ENCODINGTIME As String = "WM/EncodingTime"
        Public Const WM_GENRE As String = "WM/Genre"
        Public Const WM_INITIALKEY As String = "WM/InitialKey"
        Public Const WM_ISRC As String = "WM/ISRC"
        Public Const WM_LANGUAGE As String = "WM/Language"
        Public Const WM_LYRICS As String = "WM/Lyrics"
        Public Const WM_MOOD As String = "WM/Mood"
        Public Const WM_PARENTALRATING As String = "WM/ParentalRating"
        Public Const WM_PARTOFSET As String = "WM/PartOfSet"
        Public Const WM_PERIOD As String = "WM/Period"
        Public Const WM_PRODUCER As String = "WM/Producer"
        Public Const WM_PROMOTIONURL As String = "WM/PromotionURL"
        Public Const WM_PROVIDER As String = "WM/Provider"
        Public Const WM_PROVIDERCOPYRIGHT As String = "WM/ProviderCopyright"
        Public Const WM_PROVIDERRATING As String = "WM/ProviderRating"
        Public Const WM_PUBLISHER As String = "WM/Publisher"
        Public Const WM_SDBRATING As String = "SDB/Rating"
        Public Const WM_SHAREDUSERRATING As String = "WM/SharedUserRating"
        Public Const WM_TITLE As String = "WM/Title"
        Public Const WM_TOOLNAME As String = "WM/ToolName"
        Public Const WM_TOOLVERSION As String = "WM/ToolVersion"
        Public Const WM_TRACK As String = "WM/Track"
        Public Const WM_TRACKNUMBER As String = "WM/TrackNumber"
        Public Const WM_USERWEBURL As String = "WM/UserWebURL"
        Public Const WM_WRITER As String = "WM/Writer"
        Public Const WM_YEAR As String = "WM/Year"
#End Region

#Region "Native Methods"
        Private Const DLL As String = "AudioGenie4.dll"

        <DllImport(DLL, EntryPoint:="AACGetBitRateTypeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AACGetBitRateTypeW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AACGetHeaderTypeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AACGetHeaderTypeW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AACGetProfileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AACGetProfileW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEExistsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEExistsW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="APEGetAlbumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetAlbumW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEGetArtistW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetArtistW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEGetCommentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetCommentW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEGetCopyrightW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetCopyrightW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEGetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetGenreW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEGetItemKeysW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetItemKeysW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEGetSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetSizeW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="APEGetTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetTitleW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEGetTrackW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetTrackW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEGetUserItemW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetUserItemW(Key As String) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEGetVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetVersionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APEGetYearW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APEGetYearW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="APERemoveTagFromFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APERemoveTagFromFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="APERemoveTagW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APERemoveTagW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="APESaveChangesToFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APESaveChangesToFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="APESaveChangesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function APESaveChangesW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="APESetAlbumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub APESetAlbumW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="APESetArtistW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub APESetArtistW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="APESetCommentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub APESetCommentW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="APESetCopyrightW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub APESetCopyrightW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="APESetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub APESetGenreW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="APESetTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub APESetTitleW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="APESetTrackW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub APESetTrackW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="APESetUserItemW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub APESetUserItemW(Key As String, textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="APESetYearW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub APESetYearW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="AUDIOAnalyzeFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOAnalyzeFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOIsValidFormatW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOIsValidFormatW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetAlbumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetAlbumW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetArtistW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetArtistW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetBitrateW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetBitrateW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetChannelModeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetChannelModeW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetChannelsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetChannelsW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetCommentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetCommentW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetComposerW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetComposerW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetDurationMillisW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetDurationMillisW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetDurationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetDurationW() As Single
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetFileSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetFileSizeW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetGenreW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetLastErrorNumberW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetLastErrorNumberW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetLastErrorTextW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetLastErrorTextW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetLastFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetLastFileW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetMD5ValueW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetMD5ValueW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetSampleRateW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetSampleRateW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetTitleW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetTrackW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetTrackW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetVersionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOGetYearW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOGetYearW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOSaveChangesToFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOSaveChangesToFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOSaveChangesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function AUDIOSaveChangesW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="AUDIOSetAlbumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub AUDIOSetAlbumW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="AUDIOSetArtistW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub AUDIOSetArtistW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="AUDIOSetCommentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub AUDIOSetCommentW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="AUDIOSetComposerW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub AUDIOSetComposerW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="AUDIOSetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub AUDIOSetGenreW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="AUDIOSetTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub AUDIOSetTitleW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="AUDIOSetTrackW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub AUDIOSetTrackW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="AUDIOSetYearW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub AUDIOSetYearW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="SetPopupKeyW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub SetPopupKeyW(keyValue As String)
        End Sub
        <DllImport(DLL, EntryPoint:="FLACGetBitsPerSampleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetBitsPerSampleW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetCompressionRatioW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetCompressionRatioW() As Single
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetItemKeysW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetItemKeysW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetMaxBlockSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetMaxBlockSizeW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetMaxFrameSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetMaxFrameSizeW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetMinBlockSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetMinBlockSizeW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetMinFrameSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetMinFrameSizeW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetSamplesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetSamplesW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetUserItemW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetUserItemW(ItemKey As String) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetVendorW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetVendorW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="FLACSaveChangesToFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACSaveChangesToFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="FLACSaveChangesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACSaveChangesW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="FLACSetUserItemW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub FLACSetUserItemW(ItemKey As String, textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="FLACSetVendorW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub FLACSetVendorW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="FLACAddPictureArrayW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACAddPictureArrayW(arr As Byte(), maxLen As Integer, Description As String, PictureType As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="FLACAddPictureFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACAddPictureFileW(FileName As String, Description As String, PictureType As Short, asLink As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="FLACDeletePictureW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACDeletePictureW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="FLACDeletePicturesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub FLACDeletePicturesW()
        End Sub
        <DllImport(DLL, EntryPoint:="FLACGetPictureArrayW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetPictureArrayW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetPictureCountW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetPictureCountW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetPictureDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetPictureDescriptionW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetPictureFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetPictureFileW(FileName As String, Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetPictureMimeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetPictureMimeW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetPictureSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetPictureSizeW(Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetPictureTypeTextW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetPictureTypeTextW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="FLACGetPictureTypeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function FLACGetPictureTypeW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="GetAudioGenieVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function GetAudioGenieVersionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="GetConfigValueW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function GetConfigValueW(key As Integer) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="GetMD5ValueFromFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function GetMD5ValueFromFileW(FileName As String) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1ExistsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1ExistsW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetAlbumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetAlbumW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetArtistW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetArtistW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetCommentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetCommentW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetGenreIDW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetGenreIDW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetGenreItemW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetGenreItemW(number As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetGenreW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetGenresW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetGenresW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetTitleW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetTrackW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetTrackW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetVersionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1GetYearW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1GetYearW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1RemoveTagFromFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1RemoveTagFromFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1RemoveTagW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1RemoveTagW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1SaveChangesToFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1SaveChangesToFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1SaveChangesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V1SaveChangesW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V1SetAlbumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V1SetAlbumW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V1SetArtistW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V1SetArtistW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V1SetCommentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V1SetCommentW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V1SetGenreIDW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V1SetGenreIDW(nNewValue As Short)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V1SetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V1SetGenreW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V1SetTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V1SetTitleW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V1SetTrackW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V1SetTrackW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V1SetYearW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V1SetYearW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V2ExistsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2ExistsW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetTextFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetTextFrameW(FrameID As ID3V2FRAMES) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetTextFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V2SetTextFrameW(FrameID As ID3V2FRAMES, textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V2GetURLFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetURLFrameW(FrameID As ID3V2FRAMES) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetURLFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V2SetURLFrameW(FrameID As ID3V2FRAMES, textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V2GetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetGenreW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V2SetGenreW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V2GetPlayCounterW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPlayCounterW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetPlayCounterW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V2SetPlayCounterW(counter As Integer)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V2GetFrameCountW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetFrameCountW(FrameID As ID3V2FRAMES) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSizeW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetVersionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAllFrameIDsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAllFrameIDsW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2RemoveTagFromFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2RemoveTagFromFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2RemoveTagW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2RemoveTagW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SaveChangesToFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2SaveChangesToFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SaveChangesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2SaveChangesW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetEncodingW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetEncodingW(FrameID As ID3V2FRAMES) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetFormatAndEncodingW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2SetFormatAndEncodingW(format As Short, encoding As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSeekOffsetW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSeekOffsetW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddSeekOffsetW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V2AddSeekOffsetW(offset As Integer)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V2AddCommentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddCommentW(Language As String, Description As String, Text As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommentW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommentDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommentDescriptionW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommentLanguageW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommentLanguageW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddLyricW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddLyricW(Language As String, Description As String, Text As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetLyricW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetLyricW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetLyricDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetLyricDescriptionW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetLyricLanguageW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetLyricLanguageW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddPictureArrayW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddPictureArrayW(arr As Byte(), maxLen As Integer, Description As String, PictureType As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddPictureFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddPictureFileW(FileName As String, Description As String, PictureType As Short, asLink As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPictureArrayW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPictureArrayW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPictureDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPictureDescriptionW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPictureFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPictureFileW(FileName As String, Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPictureMimeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPictureMimeW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPictureSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPictureSizeW(Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPictureTypeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPictureTypeW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPictureTypeTextW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPictureTypeTextW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2DeleteAllFramesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2DeleteAllFramesW(FrameID As ID3V2FRAMES) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2DeleteSelectedFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2DeleteSelectedFrameW(FrameID As ID3V2FRAMES, Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddUserTextW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddUserTextW(Description As String, Text As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetUserTextW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetUserTextW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetUserTextDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetUserTextDescriptionW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddUserURLW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddUserURLW(Description As String, URL As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetUserURLW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetUserURLW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetUserURLDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetUserURLDescriptionW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddUserFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddUserFrameW(Language As String, Text As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetUserFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetUserFrameW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetUserFrameLanguageW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetUserFrameLanguageW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddPopularimeterW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddPopularimeterW(Email As String, Rating As Short, Counter As Integer) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPopularimeterEmailW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPopularimeterEmailW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPopularimeterRatingW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPopularimeterRatingW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPopularimeterCounterW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPopularimeterCounterW(Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddPrivateFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddPrivateFrameW(arr As Byte(), maxLen As Integer, URL As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPrivateFrameDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPrivateFrameDataW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPrivateFrameURLW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPrivateFrameURLW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddGeneralObjectW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddGeneralObjectW(arr As Byte(), maxLen As Integer, Mime As String, FileName As String, Description As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetGeneralObjectDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetGeneralObjectDataW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetGeneralObjectDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetGeneralObjectDescriptionW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetGeneralObjectFilenameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetGeneralObjectFilenameW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetGeneralObjectMimeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetGeneralObjectMimeW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddUniqueFileIdentifierW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddUniqueFileIdentifierW(arr As Byte(), maxLen As Integer, Owner As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetUniqueFileIdentifierDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetUniqueFileIdentifierDataW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetUniqueFileIdentifierOwnerW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetUniqueFileIdentifierOwnerW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddMusicCdIdentifierW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddMusicCdIdentifierW(arr As Byte(), maxLen As Integer) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetMusicCdIdentifierDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetMusicCdIdentifierDataW(arr As Byte(), maxLen As Integer) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddSyncLyricW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddSyncLyricW(Language As String, Description As String, Text As String, ContentType As Short, TimeStampFormat As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSyncLyricW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSyncLyricW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSyncLyricDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSyncLyricDescriptionW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSyncLyricLanguageW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSyncLyricLanguageW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSyncLyricContentTypeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSyncLyricContentTypeW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSyncLyricTimeFormatW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSyncLyricTimeFormatW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddChapterW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddChapterW(ID As String, Title As String, Description As String, startTime As Integer, endTime As Integer) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetChapterStartTimeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetChapterStartTimeW(ID As String) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetChapterEndTimeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetChapterEndTimeW(ID As String) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetChapterTimesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub ID3V2SetChapterTimesW(ID As String, startTime As Integer, endTime As Integer)
        End Sub
        <DllImport(DLL, EntryPoint:="ID3V2AddTableOfContentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddTableOfContentW(ID As String, Title As String, Description As String, isOrdered As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddChildElementW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddChildElementW(ParentTocID As String, ChildID As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2DeleteChildElementW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2DeleteChildElementW(ParentTocID As String, ChildID As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetChildElementIDW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetChildElementIDW(ID As String, Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetChildElementsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetChildElementsW(ID As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetTOCRootIDW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetTOCRootIDW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetTOCIsOrderedW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetTOCIsOrderedW(ID As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetTOCIsOrderedW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2SetTOCIsOrderedW(ID As String, isOrdered As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPossibleCHAPIDW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPossibleCHAPIDW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPossibleTOCIDW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPossibleTOCIDW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2DeleteAddendumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2DeleteAddendumW(ID As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAddendumDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAddendumDescriptionW(ID As String) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAddendumTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAddendumTitleW(ID As String) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAddendumTypeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAddendumTypeW(ID As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetAddendumDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2SetAddendumDescriptionW(ID As String, newDescription As String) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetAddendumTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2SetAddendumTitleW(ID As String, newTitle As String) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSubFramesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSubFramesW(ID As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSubFrameIDW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSubFrameIDW(ID As String, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSubFrameTextW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSubFrameTextW(ID As String, Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSubFrameDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSubFrameDescriptionW(ID As String, Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSubFrameTypeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSubFrameTypeW(ID As String, Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSubFrameImageTypeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSubFrameImageTypeW(ID As String, Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSubFrameImageW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSubFrameImageW(arr As Byte(), maxLen As Integer, ID As String, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2DeleteSubFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2DeleteSubFrameW(ID As String, FrameID As ID3V2FRAMES) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetTextSubFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2SetTextSubFrameW(ID As String, FrameID As ID3V2FRAMES, textString As String, description As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetURLSubFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2SetURLSubFrameW(ID As String, FrameID As ID3V2FRAMES, urlString As String, description As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2SetImageSubFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2SetImageSubFrameW(arr As Byte(), maxLen As Integer, ID As String, Description As String, pictureType As PictureType) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2ImportCueFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2ImportCueFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddAudioEncryptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddAudioEncryptionW(arr As Byte(), maxLen As Integer, URL As String, PreviewStart As Short, PreviewLength As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAudioEncryptionDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAudioEncryptionDataW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAudioEncryptionPreviewLengthW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAudioEncryptionPreviewLengthW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAudioEncryptionPreviewStartW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAudioEncryptionPreviewStartW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAudioEncryptionURLW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAudioEncryptionURLW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddAudioSeekPointW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddAudioSeekPointW(arr As Byte(), maxLen As Integer, start As Integer, length As Integer, numbers As Short, BitsPerPoint As Byte) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAudioSeekPointBitsPerIndexpointW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAudioSeekPointBitsPerIndexpointW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAudioSeekPointDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAudioSeekPointDataW(arr As Byte(), maxLen As Integer) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAudioSeekPointLengthW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAudioSeekPointLengthW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAudioSeekPointNumberW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAudioSeekPointNumberW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetAudioSeekPointStartW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetAudioSeekPointStartW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddCommercialFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddCommercialFrameW(arr As Byte(), maxLen As Integer, Price As String, validUntil As String, contactUrl As String, receivedAs As Short, seller As String, description As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommercialFrameContactURLW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommercialFrameContactURLW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommercialFrameDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommercialFrameDescriptionW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommercialFramePictureMimeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommercialFramePictureMimeW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommercialFramePictureW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommercialFramePictureW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommercialFramePriceW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommercialFramePriceW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommercialFrameReceivedAsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommercialFrameReceivedAsW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommercialFrameSellerNameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommercialFrameSellerNameW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetCommercialFrameValidUntilW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetCommercialFrameValidUntilW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddEncryptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddEncryptionW(arr As Byte(), maxLen As Integer, URL As String, Symbol As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetEncryptionDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetEncryptionDataW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetEncryptionSymbolW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetEncryptionSymbolW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetEncryptionURLW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetEncryptionURLW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddEqualisationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddEqualisationW(arr As Byte(), maxLen As Integer, Interpolationmethod As Byte, Identification As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetEqualisationAdjustmentBitsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetEqualisationAdjustmentBitsW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetEqualisationDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetEqualisationDataW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetEqualisationIdentificationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetEqualisationIdentificationW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetEqualisationInterpolationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetEqualisationInterpolationW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddEventTimingCodesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddEventTimingCodesW(arr As Byte(), maxLen As Integer, TimestampFormat As Byte) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetEventTimingCodesDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetEventTimingCodesDataW(arr As Byte(), maxLen As Integer) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetEventTimingCodesTimestampFormatW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetEventTimingCodesTimestampFormatW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddGroupIdentificationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddGroupIdentificationW(arr As Byte(), maxLen As Integer, Url As String, symbol As Byte) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetGroupIdentificationDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetGroupIdentificationDataW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetGroupIdentificationSymbolW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetGroupIdentificationSymbolW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetGroupIdentificationURLW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetGroupIdentificationURLW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddLinkedInformationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddLinkedInformationW(FrameIdentifier As Integer, URL As String, additionalData As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetLinkedInformationAdditionalDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetLinkedInformationAdditionalDataW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetLinkedInformationFrameIdentifierW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetLinkedInformationFrameIdentifierW(Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetLinkedInformationURLW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetLinkedInformationURLW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddMpegLocationLookupTableW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddMpegLocationLookupTableW(arr As Byte(), maxLen As Integer, Frames As Integer, Bytes As Integer, Milliseconds As Integer, BytesDeviation As Byte, MillisecondsDeviation As Byte) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetMpegLocationLookupTableBytesDeviationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetMpegLocationLookupTableBytesDeviationW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetMpegLocationLookupTableBytesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetMpegLocationLookupTableBytesW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetMpegLocationLookupTableDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetMpegLocationLookupTableDataW(arr As Byte(), maxLen As Integer) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetMpegLocationLookupTableFramesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetMpegLocationLookupTableFramesW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetMpegLocationLookupTableMillisecondsDeviationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetMpegLocationLookupTableMillisecondsDeviationW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetMpegLocationLookupTableMillisecondsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetMpegLocationLookupTableMillisecondsW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddOwnershipW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddOwnershipW(Price As String, dateString As String, Seller As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetOwnershipDateW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetOwnershipDateW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetOwnershipPriceW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetOwnershipPriceW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetOwnershipSellerW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetOwnershipSellerW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddPositionSynchronisationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddPositionSynchronisationW(TimestampFormat As Byte, Position As Integer) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPositionSynchronisationTimestampFormatW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPositionSynchronisationTimestampFormatW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetPositionSynchronisationValueW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetPositionSynchronisationValueW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddRecommendedBufferSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddRecommendedBufferSizeW(BufferSize As Integer, EmbeddedInfoFlag As Byte, Offset As Integer) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetRecommendedBufferSizeFlagW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetRecommendedBufferSizeFlagW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetRecommendedBufferSizeOffsetW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetRecommendedBufferSizeOffsetW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetRecommendedBufferSizeValueW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetRecommendedBufferSizeValueW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddRelativeVolumeAdjustmentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddRelativeVolumeAdjustmentW(arr As Byte(), maxLen As Integer, Identifier As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetRelativeVolumeAdjustmentDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetRelativeVolumeAdjustmentDataW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetRelativeVolumeAdjustmentIdentifierW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetRelativeVolumeAdjustmentIdentifierW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddReverbW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddReverbW(reverbLeft As Short, reverbRight As Short, bouncesLeft As Byte, bouncesRight As Byte, feedbackLeftToLeft As Byte, feedbackLeftToRight As Byte, feedbackRightToRight As Byte, feedbackRightToLeft As Byte, premixLeftToRight As Byte, premixRightToLeft As Byte) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetReverbBouncesLeftW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetReverbBouncesLeftW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetReverbBouncesRightW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetReverbBouncesRightW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetReverbFeedbackLeftToLeftW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetReverbFeedbackLeftToLeftW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetReverbFeedbackLeftToRightW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetReverbFeedbackLeftToRightW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetReverbFeedbackRightToLeftW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetReverbFeedbackRightToLeftW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetReverbFeedbackRightToRightW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetReverbFeedbackRightToRightW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetReverbLeftW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetReverbLeftW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetReverbRightW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetReverbRightW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetReverbPremixLeftToRightW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetReverbPremixLeftToRightW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetReverbPremixRightToLeftW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetReverbPremixRightToLeftW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddSignatureFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddSignatureFrameW(arr As Byte(), maxLen As Integer, GroupSymbol As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSignatureFrameDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSignatureFrameDataW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSignatureFrameGroupSymbolW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSignatureFrameGroupSymbolW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2AddSynchronizedTempoW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2AddSynchronizedTempoW(arr As Byte(), maxLen As Integer, Format As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSynchronizedTempoDataW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSynchronizedTempoDataW(arr As Byte(), maxLen As Integer) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="ID3V2GetSynchronizedTempoFormatW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function ID3V2GetSynchronizedTempoFormatW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSExistsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSExistsW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetAlbumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetAlbumW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetArtistW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetArtistW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetAuthorW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetAuthorW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetGenreW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetImageLinkW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetImageLinkW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetIndicationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetIndicationW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetInformationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetInformationW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetLyricsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetLyricsW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetSizeW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetStartPositionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetStartPositionW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetTitleW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSGetVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSGetVersionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSRemoveTagFromFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSRemoveTagFromFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSRemoveTagW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSRemoveTagW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSSaveChangesToFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSSaveChangesToFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSSaveChangesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function LYRICSSaveChangesW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="LYRICSSetAlbumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub LYRICSSetAlbumW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="LYRICSSetArtistW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub LYRICSSetArtistW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="LYRICSSetAuthorW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub LYRICSSetAuthorW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="LYRICSSetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub LYRICSSetGenreW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="LYRICSSetImageLinkW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub LYRICSSetImageLinkW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="LYRICSSetIndicationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub LYRICSSetIndicationW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="LYRICSSetInformationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub LYRICSSetInformationW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="LYRICSSetLyricsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub LYRICSSetLyricsW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="LYRICSSetTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub LYRICSSetTitleW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="MONKEYGetBitsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MONKEYGetBitsW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="MONKEYGetCompressionRatioW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MONKEYGetCompressionRatioW() As Single
        End Function
        <DllImport(DLL, EntryPoint:="MONKEYGetCompressionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MONKEYGetCompressionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MONKEYGetFramesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MONKEYGetFramesW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="MONKEYGetPeakW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MONKEYGetPeakW() As Single
        End Function
        <DllImport(DLL, EntryPoint:="MONKEYGetSamplesPerFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MONKEYGetSamplesPerFrameW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="MONKEYGetSamplesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MONKEYGetSamplesW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="MP4DeleteEntriesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub MP4DeleteEntriesW()
        End Sub
        <DllImport(DLL, EntryPoint:="MP4DeletePictureW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub MP4DeletePictureW(Index As Short)
        End Sub
        <DllImport(DLL, EntryPoint:="MP4DeletePicturesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub MP4DeletePicturesW()
        End Sub
        <DllImport(DLL, EntryPoint:="MP4GetAllFrameIDsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetAllFrameIDsW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MP4GetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetGenreW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MP4GetTextFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetTextFrameW(FrameID As MP4FRAMES) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MP4GetTrackW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetTrackW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MP4GetVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetVersionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MP4GetiTuneFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetiTuneFrameW(FrameID As String) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MP4GetPictureArrayW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetPictureArrayW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="MP4GetPictureCountW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetPictureCountW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="MP4GetPictureFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetPictureFileW(FileName As String, Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="MP4GetPictureMimeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetPictureMimeW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MP4GetPictureSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4GetPictureSizeW(Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="MP4SaveChangesToFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4SaveChangesToFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="MP4SaveChangesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4SaveChangesW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="MP4SetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub MP4SetGenreW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="MP4SetTextFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub MP4SetTextFrameW(FrameID As MP4FRAMES, textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="MP4SetTrackW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub MP4SetTrackW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="MP4SetiTuneFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub MP4SetiTuneFrameW(FrameID As String, textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="MP4AddPictureArrayW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4AddPictureArrayW(arr As Byte(), maxLen As Integer) As Short
        End Function
        <DllImport(DLL, EntryPoint:="MP4AddPictureFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MP4AddPictureFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="MPEGGetEmphasisW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGGetEmphasisW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MPEGGetEncoderW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGGetEncoderW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MPEGGetFramePositionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGGetFramePositionW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="MPEGGetFrameSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGGetFrameSizeW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="MPEGGetFramesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGGetFramesW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="MPEGGetLayerW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGGetLayerW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MPEGGetVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGGetVersionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="MPEGIsCopyrightedW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGIsCopyrightedW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="MPEGIsOriginalW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGIsOriginalW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="MPEGIsPaddingW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGIsPaddingW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="MPEGIsPrivateW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGIsPrivateW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="MPEGIsProtectedW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGIsProtectedW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="MPEGIsVBRW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGIsVBRW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="MPEGSetCopyrightedW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGSetCopyrightedW(FileName As String, newValue As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="MPEGSetOriginalW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGSetOriginalW(FileName As String, newValue As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="MPEGSetPrivateW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPEGSetPrivateW(FileName As String, newValue As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="MPPGetFramesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPPGetFramesW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="MPPGetStreamVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function MPPGetStreamVersionW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetAlbumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetAlbumW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetArtistW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetArtistW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetBitRateNominalW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetBitRateNominalW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetCommentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetCommentW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetContactW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetContactW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetCopyrightW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetCopyrightW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetDateW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetDateW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetDescriptionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetGenreW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetISRCW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetISRCW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetItemKeysW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetItemKeysW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetLicenseW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetLicenseW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetLocationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetLocationW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetOrganizationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetOrganizationW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetPerformerW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetPerformerW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetTitleW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetTrackW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetTrackW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetUserItemW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetUserItemW(ItemKey As String) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetVendorW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetVendorW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGGetVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGGetVersionW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="OGGRemoveTagFromFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGRemoveTagFromFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="OGGRemoveTagW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGRemoveTagW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="OGGSaveChangesToFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGSaveChangesToFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="OGGSaveChangesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function OGGSaveChangesW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="OGGSetAlbumW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetAlbumW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetArtistW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetArtistW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetCommentW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetCommentW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetContactW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetContactW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetCopyrightW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetCopyrightW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetDateW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetDateW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetDescriptionW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetGenreW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetGenreW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetISRCW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetISRCW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetLicenseW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetLicenseW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetLocationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetLocationW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetOrganizationW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetOrganizationW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetPerformerW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetPerformerW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetTitleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetTitleW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetTrackW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetTrackW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetUserItemW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetUserItemW(ItemKey As String, textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetVendorW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetVendorW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="OGGSetVersionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub OGGSetVersionW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="WAVGetBitsPerSampleW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetBitsPerSampleW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="WAVGetBlockAlignW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetBlockAlignW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="WAVGetBytesPerSecondW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetBytesPerSecondW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="WAVGetChannelsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetChannelsW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="WAVGetDisplayTextW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetDisplayTextW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="WAVGetFormatIDW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetFormatIDW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="WAVGetFormatW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetFormatW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="WAVGetHeaderSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetHeaderSizeW() As Integer
        End Function
        <DllImport(DLL, EntryPoint:="WAVGetInfoChunkIDsW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetInfoChunkIDsW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="WAVGetTextFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetTextFrameW(FrameID As WAVFRAMES) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="WAVSetTextFrameW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub WAVSetTextFrameW(FrameID As WAVFRAMES, textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="WAVSetDisplayTextW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub WAVSetDisplayTextW(textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="WAVGetCartChunkEntryW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVGetCartChunkEntryW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="WAVSetCartChunkEntryW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub WAVSetCartChunkEntryW(Index As Short, textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="WAVSaveChangesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVSaveChangesW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="WAVSaveChangesToFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WAVSaveChangesToFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="WMADeletePictureW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMADeletePictureW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="WMADeletePicturesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub WMADeletePicturesW()
        End Sub
        <DllImport(DLL, EntryPoint:="WMAGetItemKeysW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAGetItemKeysW() As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="WMAGetPictureArrayW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAGetPictureArrayW(arr As Byte(), maxLen As Integer, Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="WMAGetPictureCountW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAGetPictureCountW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="WMAGetPictureDescriptionW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAGetPictureDescriptionW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="WMAGetPictureFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAGetPictureFileW(FileName As String, Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="WMAGetPictureMimeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAGetPictureMimeW(Index As Short) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="WMAGetPictureSizeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAGetPictureSizeW(Index As Short) As Integer
        End Function
        <DllImport(DLL, EntryPoint:="WMAGetPictureTypeW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAGetPictureTypeW(Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="WMAGetUserItemW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAGetUserItemW(ItemKey As String) As IntPtr
        End Function
        <DllImport(DLL, EntryPoint:="WMAIsVBRW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAIsVBRW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="WMASaveChangesToFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMASaveChangesToFileW(FileName As String) As Short
        End Function
        <DllImport(DLL, EntryPoint:="WMASaveChangesW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMASaveChangesW() As Short
        End Function
        <DllImport(DLL, EntryPoint:="WMAAddPictureArrayW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAAddPictureArrayW(arr As Byte(), maxLen As Integer, Description As String, PictureType As Short, Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="WMAAddPictureFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Function WMAAddPictureFileW(FileName As String, Description As String, PictureType As Short, Index As Short) As Short
        End Function
        <DllImport(DLL, EntryPoint:="WMASetUserItemW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub WMASetUserItemW(ItemKey As String, textString As String)
        End Sub
        <DllImport(DLL, EntryPoint:="SetConfigValueW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub SetConfigValueW(key As Integer, value As Integer)
        End Sub
        <DllImport(DLL, EntryPoint:="SetLogFileW", CharSet:=CharSet.Unicode, SetLastError:=True)>
        Private Shared Sub SetLogFileW(fileName As String)
        End Sub
#End Region

#Region "APE Properties"
        Public Shared Property APEAlbum As String
            Get
                Return SafeStr(APEGetAlbumW())
            End Get
            Set(value As String)
                APESetAlbumW(value)
            End Set
        End Property
        Public Shared Property APEArtist As String
            Get
                Return SafeStr(APEGetArtistW())
            End Get
            Set(value As String)
                APESetArtistW(value)
            End Set
        End Property
        Public Shared Property APEComment As String
            Get
                Return SafeStr(APEGetCommentW())
            End Get
            Set(value As String)
                APESetCommentW(value)
            End Set
        End Property
        Public Shared Property APECopyright As String
            Get
                Return SafeStr(APEGetCopyrightW())
            End Get
            Set(value As String)
                APESetCopyrightW(value)
            End Set
        End Property
        Public Shared Property APEGenre As String
            Get
                Return SafeStr(APEGetGenreW())
            End Get
            Set(value As String)
                APESetGenreW(value)
            End Set
        End Property
        Public Shared Property APETitle As String
            Get
                Return SafeStr(APEGetTitleW())
            End Get
            Set(value As String)
                APESetTitleW(value)
            End Set
        End Property
        Public Shared Property APETrack As String
            Get
                Return SafeStr(APEGetTrackW())
            End Get
            Set(value As String)
                APESetTrackW(value)
            End Set
        End Property
        Public Shared Property APEYear As String
            Get
                Return SafeStr(APEGetYearW())
            End Get
            Set(value As String)
                APESetYearW(value)
            End Set
        End Property
#End Region

#Region "AUDIO Properties"
        Public Shared Property AUDIOAlbum As String
            Get
                Return SafeStr(AUDIOGetAlbumW())
            End Get
            Set(value As String)
                AUDIOSetAlbumW(value)
            End Set
        End Property
        Public Shared Property AUDIOArtist As String
            Get
                Return SafeStr(AUDIOGetArtistW())
            End Get
            Set(value As String)
                AUDIOSetArtistW(value)
            End Set
        End Property
        Public Shared Property AUDIOComment As String
            Get
                Return SafeStr(AUDIOGetCommentW())
            End Get
            Set(value As String)
                AUDIOSetCommentW(value)
            End Set
        End Property
        Public Shared Property AUDIOComposer As String
            Get
                Return SafeStr(AUDIOGetComposerW())
            End Get
            Set(value As String)
                AUDIOSetComposerW(value)
            End Set
        End Property
        Public Shared Property AUDIOGenre As String
            Get
                Return SafeStr(AUDIOGetGenreW())
            End Get
            Set(value As String)
                AUDIOSetGenreW(value)
            End Set
        End Property
        Public Shared Property AUDIOTitle As String
            Get
                Return SafeStr(AUDIOGetTitleW())
            End Get
            Set(value As String)
                AUDIOSetTitleW(value)
            End Set
        End Property
        Public Shared Property AUDIOTrack As String
            Get
                Return SafeStr(AUDIOGetTrackW())
            End Get
            Set(value As String)
                AUDIOSetTrackW(value)
            End Set
        End Property
        Public Shared Property AUDIOYear As String
            Get
                Return SafeStr(AUDIOGetYearW())
            End Get
            Set(value As String)
                AUDIOSetYearW(value)
            End Set
        End Property
#End Region

#Region "FLAC Properties"
        Public Shared Property FLACVendor As String
            Get
                Return SafeStr(FLACGetVendorW())
            End Get
            Set(value As String)
                FLACSetVendorW(value)
            End Set
        End Property
#End Region

#Region "ID3V1 Properties"
        Public Shared Property ID3V1Album As String
            Get
                Return SafeStr(ID3V1GetAlbumW())
            End Get
            Set(value As String)
                ID3V1SetAlbumW(value)
            End Set
        End Property
        Public Shared Property ID3V1Artist As String
            Get
                Return SafeStr(ID3V1GetArtistW())
            End Get
            Set(value As String)
                ID3V1SetArtistW(value)
            End Set
        End Property
        Public Shared Property ID3V1Comment As String
            Get
                Return SafeStr(ID3V1GetCommentW())
            End Get
            Set(value As String)
                ID3V1SetCommentW(value)
            End Set
        End Property
        Public Shared Property ID3V1Genre As String
            Get
                Return SafeStr(ID3V1GetGenreW())
            End Get
            Set(value As String)
                ID3V1SetGenreW(value)
            End Set
        End Property
        Public Shared Property ID3V1GenreID As Short
            Get
                Return ID3V1GetGenreIDW()
            End Get
            Set(value As Short)
                ID3V1SetGenreIDW(value)
            End Set
        End Property
        Public Shared Property ID3V1Title As String
            Get
                Return SafeStr(ID3V1GetTitleW())
            End Get
            Set(value As String)
                ID3V1SetTitleW(value)
            End Set
        End Property
        Public Shared Property ID3V1Track As String
            Get
                Return SafeStr(ID3V1GetTrackW())
            End Get
            Set(value As String)
                ID3V1SetTrackW(value)
            End Set
        End Property
        Public Shared Property ID3V1Year As String
            Get
                Return SafeStr(ID3V1GetYearW())
            End Get
            Set(value As String)
                ID3V1SetYearW(value)
            End Set
        End Property
#End Region

#Region "ID3V2 Properties"
        Public Shared Property ID3V2Album As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TALB)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TALB, value)
            End Set
        End Property
        Public Shared Property ID3V2AlbumSort As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSOA)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSOA, value)
            End Set
        End Property
        Public Shared Property ID3V2Artist As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPE1)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPE1, value)
            End Set
        End Property
        Public Shared Property ID3V2ArtistSort As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSOP)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSOP, value)
            End Set
        End Property
        Public Shared Property ID3V2ArtistURL As String
            Get
                Return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WOAR)
            End Get
            Set(value As String)
                ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WOAR, value)
            End Set
        End Property
        Public Shared Property ID3V2AudiofileURL As String
            Get
                Return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WOAF)
            End Get
            Set(value As String)
                ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WOAF, value)
            End Set
        End Property
        Public Shared Property ID3V2AudioSourceURL As String
            Get
                Return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WOAS)
            End Get
            Set(value As String)
                ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WOAS, value)
            End Set
        End Property
        Public Shared Property ID3V2BeatsPerMinute As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TBPM)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TBPM, value)
            End Set
        End Property
        Public Shared Property ID3V2Compilation As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TCMP)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TCMP, value)
            End Set
        End Property
        Public Shared Property ID3V2Composer As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TCOM)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TCOM, value)
            End Set
        End Property
        Public Shared Property ID3V2ComposerSort As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSOC)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSOC, value)
            End Set
        End Property
        Public Shared Property ID3V2Conductor As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPE3)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPE3, value)
            End Set
        End Property
        Public Shared Property ID3V2ContentGroupDescription As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TIT1)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TIT1, value)
            End Set
        End Property
        Public Shared Property ID3V2Copyright As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TCOP)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TCOP, value)
            End Set
        End Property
        Public Shared Property ID3V2CopyrightURL As String
            Get
                Return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WCOP)
            End Get
            Set(value As String)
                ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WCOP, value)
            End Set
        End Property
        Public Shared Property ID3V2CommercialURL As String
            Get
                Return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WCOM)
            End Get
            Set(value As String)
                ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WCOM, value)
            End Set
        End Property
        Public Shared Property ID3V2Date As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDAT)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDAT, value)
            End Set
        End Property
        Public Shared Property ID3V2EncodedBy As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TENC)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TENC, value)
            End Set
        End Property
        Public Shared Property ID3V2EncodingSettings As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSSE)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSSE, value)
            End Set
        End Property
        Public Shared Property ID3V2EncodingTime As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDEN)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDEN, value)
            End Set
        End Property
        Public Shared Property ID3V2FileOwner As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TOWN)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TOWN, value)
            End Set
        End Property
        Public Shared Property ID3V2FileType As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TFLT)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TFLT, value)
            End Set
        End Property
        Public Shared Property ID3V2Genre As String
            Get
                Return SafeStr(ID3V2GetGenreW())
            End Get
            Set(value As String)
                ID3V2SetGenreW(value)
            End Set
        End Property
        Public Shared Property ID3V2InetRadioName As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TRSN)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TRSN, value)
            End Set
        End Property
        Public Shared Property ID3V2InetRadioOwner As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TRSO)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TRSO, value)
            End Set
        End Property
        Public Shared Property ID3V2InitialKey As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TKEY)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TKEY, value)
            End Set
        End Property
        Public Shared Property ID3V2Interpreted As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPE4)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPE4, value)
            End Set
        End Property
        Public Shared Property ID3V2InvolvedPeopleList As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TIPL)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TIPL, value)
            End Set
        End Property
        Public Shared Property ID3V2ISRC As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSRC)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSRC, value)
            End Set
        End Property
        Public Shared Property ID3V2Language As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TLAN)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TLAN, value)
            End Set
        End Property
        Public Shared Property ID3V2Length As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TLEN)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TLEN, value)
            End Set
        End Property
        Public Shared Property ID3V2MediaType As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TMED)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TMED, value)
            End Set
        End Property
        Public Shared Property ID3V2Mood As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TMOO)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TMOO, value)
            End Set
        End Property
        Public Shared Property ID3V2MusicianCreditsList As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TMCL)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TMCL, value)
            End Set
        End Property
        Public Shared Property ID3V2Orchestra As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPE2)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPE2, value)
            End Set
        End Property
        Public Shared Property ID3V2OriginalAlbum As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TOAL)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TOAL, value)
            End Set
        End Property
        Public Shared Property ID3V2OriginalArtist As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TOPE)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TOPE, value)
            End Set
        End Property
        Public Shared Property ID3V2OriginalFilename As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TOFN)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TOFN, value)
            End Set
        End Property
        Public Shared Property ID3V2OriginalReleaseTime As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDOR)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDOR, value)
            End Set
        End Property
        Public Shared Property ID3V2OriginalReleaseYear As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TORY)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TORY, value)
            End Set
        End Property
        Public Shared Property ID3V2OriginalTextwriter As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TOLY)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TOLY, value)
            End Set
        End Property
        Public Shared Property ID3V2PartofSet As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPOS)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPOS, value)
            End Set
        End Property
        Public Shared Property ID3V2PaymentURL As String
            Get
                Return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WPAY)
            End Get
            Set(value As String)
                ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WPAY, value)
            End Set
        End Property
        Public Shared Property ID3V2PlayCounter As Integer
            Get
                Return ID3V2GetPlayCounterW()
            End Get
            Set(value As Integer)
                ID3V2SetPlayCounterW(value)
            End Set
        End Property
        Public Shared Property ID3V2PlaylistDelay As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDLY)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDLY, value)
            End Set
        End Property
        Public Shared Property ID3V2ProducedNotice As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPRO)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPRO, value)
            End Set
        End Property
        Public Shared Property ID3V2Publisher As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TPUB)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TPUB, value)
            End Set
        End Property
        Public Shared Property ID3V2PublisherURL As String
            Get
                Return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WPUB)
            End Get
            Set(value As String)
                ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WPUB, value)
            End Set
        End Property
        Public Shared Property ID3V2RadiostationURL As String
            Get
                Return ID3V2GetURLFrame(ID3V2FRAMES.ID3F_WORS)
            End Get
            Set(value As String)
                ID3V2SetURLFrame(ID3V2FRAMES.ID3F_WORS, value)
            End Set
        End Property
        Public Shared Property ID3V2RecordingDate As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TRDA)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TRDA, value)
            End Set
        End Property
        Public Shared Property ID3V2RecordingTime As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDRC)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDRC, value)
            End Set
        End Property
        Public Shared Property ID3V2ReleaseTime As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDRL)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDRL, value)
            End Set
        End Property
        Public Shared Property ID3V2Size As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSIZ)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSIZ, value)
            End Set
        End Property
        Public Shared Property ID3V2Subtitle As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TIT3)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TIT3, value)
            End Set
        End Property
        Public Shared Property ID3V2SubtitleSet As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSST)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSST, value)
            End Set
        End Property
        Public Shared Property ID3V2TaggingTime As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TDTG)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TDTG, value)
            End Set
        End Property
        Public Shared Property ID3V2TextWriter As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TEXT)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TEXT, value)
            End Set
        End Property
        Public Shared Property ID3V2Time As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TIME)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TIME, value)
            End Set
        End Property
        Public Shared Property ID3V2Title As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TIT2)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TIT2, value)
            End Set
        End Property
        Public Shared Property ID3V2TitleSort As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TSOT)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TSOT, value)
            End Set
        End Property
        Public Shared Property ID3V2Track As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TRCK)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TRCK, value)
            End Set
        End Property
        Public Shared Property ID3V2Year As String
            Get
                Return ID3V2GetTextFrame(ID3V2FRAMES.ID3F_TYER)
            End Get
            Set(value As String)
                ID3V2SetTextFrame(ID3V2FRAMES.ID3F_TYER, value)
            End Set
        End Property
#End Region

#Region "LYRICS Properties"
        Public Shared Property LYRICSAlbum As String
            Get
                Return SafeStr(LYRICSGetAlbumW())
            End Get
            Set(value As String)
                LYRICSSetAlbumW(value)
            End Set
        End Property
        Public Shared Property LYRICSArtist As String
            Get
                Return SafeStr(LYRICSGetArtistW())
            End Get
            Set(value As String)
                LYRICSSetArtistW(value)
            End Set
        End Property
        Public Shared Property LYRICSAuthor As String
            Get
                Return SafeStr(LYRICSGetAuthorW())
            End Get
            Set(value As String)
                LYRICSSetAuthorW(value)
            End Set
        End Property
        Public Shared Property LYRICSGenre As String
            Get
                Return SafeStr(LYRICSGetGenreW())
            End Get
            Set(value As String)
                LYRICSSetGenreW(value)
            End Set
        End Property
        Public Shared Property LYRICSImageLink As String
            Get
                Return SafeStr(LYRICSGetImageLinkW())
            End Get
            Set(value As String)
                LYRICSSetImageLinkW(value)
            End Set
        End Property
        Public Shared Property LYRICSIndication As String
            Get
                Return SafeStr(LYRICSGetIndicationW())
            End Get
            Set(value As String)
                LYRICSSetIndicationW(value)
            End Set
        End Property
        Public Shared Property LYRICSInformation As String
            Get
                Return SafeStr(LYRICSGetInformationW())
            End Get
            Set(value As String)
                LYRICSSetInformationW(value)
            End Set
        End Property
        Public Shared Property LYRICSLyrics As String
            Get
                Return SafeStr(LYRICSGetLyricsW())
            End Get
            Set(value As String)
                LYRICSSetLyricsW(value)
            End Set
        End Property
        Public Shared Property LYRICSTitle As String
            Get
                Return SafeStr(LYRICSGetTitleW())
            End Get
            Set(value As String)
                LYRICSSetTitleW(value)
            End Set
        End Property
#End Region

#Region "MP4 Properties"
        Public Shared Property MP4Album As String
            Get
                Return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_ALBUM))
            End Get
            Set(value As String)
                MP4SetTextFrameW(MP4FRAMES.MP4_ALBUM, value)
            End Set
        End Property
        Public Shared Property MP4Artist As String
            Get
                Return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_ARTIST))
            End Get
            Set(value As String)
                MP4SetTextFrameW(MP4FRAMES.MP4_ARTIST, value)
            End Set
        End Property
        Public Shared Property MP4Author As String
            Get
                Return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_AUTHOR))
            End Get
            Set(value As String)
                MP4SetTextFrameW(MP4FRAMES.MP4_AUTHOR, value)
            End Set
        End Property
        Public Shared Property MP4BeatsPerMinute As String
            Get
                Return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_BEATSPERMINUTE))
            End Get
            Set(value As String)
                MP4SetTextFrameW(MP4FRAMES.MP4_BEATSPERMINUTE, value)
            End Set
        End Property
        Public Shared Property MP4Comment As String
            Get
                Return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_COMMENT))
            End Get
            Set(value As String)
                MP4SetTextFrameW(MP4FRAMES.MP4_COMMENT, value)
            End Set
        End Property
        Public Shared Property MP4Composer As String
            Get
                Return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_COMPOSER))
            End Get
            Set(value As String)
                MP4SetTextFrameW(MP4FRAMES.MP4_COMPOSER, value)
            End Set
        End Property
        Public Shared Property MP4Copyright As String
            Get
                Return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_COPYRIGHT))
            End Get
            Set(value As String)
                MP4SetTextFrameW(MP4FRAMES.MP4_COPYRIGHT, value)
            End Set
        End Property
        Public Shared Property MP4Genre As String
            Get
                Return SafeStr(MP4GetGenreW())
            End Get
            Set(value As String)
                MP4SetGenreW(value)
            End Set
        End Property
        Public Shared Property MP4Title As String
            Get
                Return SafeStr(MP4GetTextFrameW(MP4FRAMES.MP4_TITLE))
            End Get
            Set(value As String)
                MP4SetTextFrameW(MP4FRAMES.MP4_TITLE, value)
            End Set
        End Property
        Public Shared Property MP4Track As String
            Get
                Return SafeStr(MP4GetTrackW())
            End Get
            Set(value As String)
                MP4SetTrackW(value)
            End Set
        End Property
#End Region

#Region "OGG Properties"
        Public Shared Property OGGAlbum As String
            Get
                Return SafeStr(OGGGetAlbumW())
            End Get
            Set(value As String)
                OGGSetAlbumW(value)
            End Set
        End Property
        Public Shared Property OGGArtist As String
            Get
                Return SafeStr(OGGGetArtistW())
            End Get
            Set(value As String)
                OGGSetArtistW(value)
            End Set
        End Property
        Public Shared Property OGGComment As String
            Get
                Return SafeStr(OGGGetCommentW())
            End Get
            Set(value As String)
                OGGSetCommentW(value)
            End Set
        End Property
        Public Shared Property OGGCopyright As String
            Get
                Return SafeStr(OGGGetCopyrightW())
            End Get
            Set(value As String)
                OGGSetCopyrightW(value)
            End Set
        End Property
        Public Shared Property OGGDate As String
            Get
                Return SafeStr(OGGGetDateW())
            End Get
            Set(value As String)
                OGGSetDateW(value)
            End Set
        End Property
        Public Shared Property OGGGenre As String
            Get
                Return SafeStr(OGGGetGenreW())
            End Get
            Set(value As String)
                OGGSetGenreW(value)
            End Set
        End Property
        Public Shared Property OGGTitle As String
            Get
                Return SafeStr(OGGGetTitleW())
            End Get
            Set(value As String)
                OGGSetTitleW(value)
            End Set
        End Property
        Public Shared Property OGGTrack As String
            Get
                Return SafeStr(OGGGetTrackW())
            End Get
            Set(value As String)
                OGGSetTrackW(value)
            End Set
        End Property
#End Region

#Region "WAV Properties"
        Public Shared Property WAVArtist As String
            Get
                Return WAVGetTextFrame(WAVFRAMES.WAV_IART)
            End Get
            Set(value As String)
                WAVSetTextFrame(WAVFRAMES.WAV_IART, value)
            End Set
        End Property
        Public Shared Property WAVComment As String
            Get
                Return WAVGetTextFrame(WAVFRAMES.WAV_ICMT)
            End Get
            Set(value As String)
                WAVSetTextFrame(WAVFRAMES.WAV_ICMT, value)
            End Set
        End Property
        Public Shared Property WAVGenre As String
            Get
                Return WAVGetTextFrame(WAVFRAMES.WAV_IGNR)
            End Get
            Set(value As String)
                WAVSetTextFrame(WAVFRAMES.WAV_IGNR, value)
            End Set
        End Property
        Public Shared Property WAVName As String
            Get
                Return WAVGetTextFrame(WAVFRAMES.WAV_INAM)
            End Get
            Set(value As String)
                WAVSetTextFrame(WAVFRAMES.WAV_INAM, value)
            End Set
        End Property
        Public Shared Property WAVDisplayText As String
            Get
                Return SafeStr(WAVGetDisplayTextW())
            End Get
            Set(value As String)
                WAVSetDisplayTextW(value)
            End Set
        End Property
#End Region

#Region "WMA Properties"
        Public Shared Property WMAAlbum As String
            Get
                Return WMAGetUserItem(WM_ALBUMTITLE)
            End Get
            Set(value As String)
                WMASetUserItem(WM_ALBUMTITLE, value)
            End Set
        End Property
        Public Shared Property WMAArtist As String
            Get
                Return WMAGetUserItem(WM_AUTHOR)
            End Get
            Set(value As String)
                WMASetUserItem(WM_AUTHOR, value)
            End Set
        End Property
        Public Shared Property WMAGenre As String
            Get
                Return WMAGetUserItem(WM_GENRE)
            End Get
            Set(value As String)
                WMASetUserItem(WM_GENRE, value)
            End Set
        End Property
        Public Shared Property WMATitle As String
            Get
                Return WMAGetUserItem(WM_TITLE)
            End Get
            Set(value As String)
                WMASetUserItem(WM_TITLE, value)
            End Set
        End Property
        Public Shared Property WMATrackNumber As String
            Get
                Return WMAGetUserItem(WM_TRACKNUMBER)
            End Get
            Set(value As String)
                WMASetUserItem(WM_TRACKNUMBER, value)
            End Set
        End Property
        Public Shared Property WMAYear As String
            Get
                Return WMAGetUserItem(WM_YEAR)
            End Get
            Set(value As String)
                WMASetUserItem(WM_YEAR, value)
            End Set
        End Property
#End Region

#Region "AAC Methods"
        Public Shared Function AACGetBitRateType() As String
            Return SafeStr(AACGetBitRateTypeW())
        End Function
        Public Shared Function AACGetHeaderType() As String
            Return SafeStr(AACGetHeaderTypeW())
        End Function
        Public Shared Function AACGetProfile() As String
            Return SafeStr(AACGetProfileW())
        End Function
#End Region

#Region "APE Methods"
        Public Shared Function APEExists() As Boolean
            Return Convert.ToBoolean(APEExistsW())
        End Function
        Public Shared Function APEGetItemKeys() As String
            Return SafeStr(APEGetItemKeysW())
        End Function
        Public Shared Function APEGetSize() As Integer
            Return APEGetSizeW()
        End Function
        Public Shared Function APEGetUserItem(Key As String) As String
            Return SafeStr(APEGetUserItemW(Key))
        End Function
        Public Shared Function APEGetVersion() As String
            Return SafeStr(APEGetVersionW())
        End Function
        Public Shared Function APERemoveTagFromFile(FileName As String) As Boolean
            Return Convert.ToBoolean(APERemoveTagFromFileW(FileName))
        End Function
        Public Shared Function APERemoveTag() As Boolean
            Return Convert.ToBoolean(APERemoveTagW())
        End Function
        Public Shared Function APESaveChangesToFile(FileName As String) As Boolean
            Return Convert.ToBoolean(APESaveChangesToFileW(FileName))
        End Function
        Public Shared Function APESaveChanges() As Boolean
            Return Convert.ToBoolean(APESaveChangesW())
        End Function
        Public Shared Sub APESetUserItem(Key As String, textString As String)
            APESetUserItemW(Key, textString)
        End Sub
#End Region

#Region "AUDIO Methods"
        Public Shared Function AUDIOAnalyzeFile(FileName As String) As AudioFormatID
            Return CType(AUDIOAnalyzeFileW(FileName), AudioFormatID)
        End Function
        Public Shared Function AUDIOIsValidFormat() As Boolean
            Return Convert.ToBoolean(AUDIOIsValidFormatW())
        End Function
        Public Shared Function AUDIOGetBitrate() As Integer
            Return AUDIOGetBitrateW()
        End Function
        Public Shared Function AUDIOGetChannelMode() As String
            Return SafeStr(AUDIOGetChannelModeW())
        End Function
        Public Shared Function AUDIOGetChannels() As Integer
            Return AUDIOGetChannelsW()
        End Function
        Public Shared Function AUDIOGetDurationMillis() As Integer
            Return AUDIOGetDurationMillisW()
        End Function
        Public Shared Function AUDIOGetDuration() As Single
            Return AUDIOGetDurationW()
        End Function
        Public Shared Function AUDIOGetFileSize() As Integer
            Return AUDIOGetFileSizeW()
        End Function
        Public Shared Function AUDIOGetLastErrorNumber() As Integer
            Return AUDIOGetLastErrorNumberW()
        End Function
        Public Shared Function AUDIOGetLastErrorText() As String
            Return SafeStr(AUDIOGetLastErrorTextW())
        End Function
        Public Shared Function AUDIOGetLastFile() As String
            Return SafeStr(AUDIOGetLastFileW())
        End Function
        Public Shared Function AUDIOGetMD5Value() As String
            Return SafeStr(AUDIOGetMD5ValueW())
        End Function
        Public Shared Function AUDIOGetSampleRate() As Integer
            Return AUDIOGetSampleRateW()
        End Function
        Public Shared Function AUDIOGetVersion() As String
            Return SafeStr(AUDIOGetVersionW())
        End Function
        Public Shared Function AUDIOSaveChangesToFile(FileName As String) As Boolean
            Return Convert.ToBoolean(AUDIOSaveChangesToFileW(FileName))
        End Function
        Public Shared Function AUDIOSaveChanges() As Boolean
            Return Convert.ToBoolean(AUDIOSaveChangesW())
        End Function
#End Region

#Region "AudioGenie Methods"
        Public Shared Function GetConfigValue(key As Integer) As Integer
            Return GetConfigValueW(key)
        End Function
        Public Shared Function GetAudioGenieVersion() As String
            Return SafeStr(GetAudioGenieVersionW())
        End Function
        Public Shared Sub SetConfigValue(key As Integer, value As Integer)
            SetConfigValueW(key, value)
        End Sub
        Public Shared Sub SetLogFile(fileName As String)
            SetLogFileW(fileName)
        End Sub
        Public Shared Sub SetPopupKey(keyValue As String)
            SetPopupKeyW(keyValue)
        End Sub
#End Region

#Region "FLAC Methods"
        Public Shared Function FLACAddPictureArray(arr As Byte(), Description As String, PictureType As Short) As Short
            Return FLACAddPictureArrayW(arr, arr.Length, Description, PictureType)
        End Function
        Public Shared Function FLACAddPictureFile(FileName As String, Description As String, PictureType As Short, asLink As Short) As Short
            Return FLACAddPictureFileW(FileName, Description, PictureType, asLink)
        End Function
        Public Shared Function FLACDeletePicture(Index As Short) As Boolean
            Return Convert.ToBoolean(FLACDeletePictureW(Index))
        End Function
        Public Shared Sub FLACDeletePictures()
            FLACDeletePicturesW()
        End Sub
        Public Shared Function FLACGetBitsPerSample() As Integer
            Return FLACGetBitsPerSampleW()
        End Function
        Public Shared Function FLACGetCompressionRatio() As Single
            Return FLACGetCompressionRatioW()
        End Function
        Public Shared Function FLACGetItemKeys() As String
            Return SafeStr(FLACGetItemKeysW())
        End Function
        Public Shared Function FLACGetMaxBlockSize() As Integer
            Return FLACGetMaxBlockSizeW()
        End Function
        Public Shared Function FLACGetMaxFrameSize() As Integer
            Return FLACGetMaxFrameSizeW()
        End Function
        Public Shared Function FLACGetMinBlockSize() As Integer
            Return FLACGetMinBlockSizeW()
        End Function
        Public Shared Function FLACGetMinFrameSize() As Integer
            Return FLACGetMinFrameSizeW()
        End Function
        Public Shared Function FLACGetPictureCount() As Short
            Return FLACGetPictureCountW()
        End Function
        Public Shared Function FLACGetPictureArray(Index As Short) As Byte()
            Dim maxLen As Integer = CInt(FLACGetPictureSize(Index))
            If maxLen <= 0 Then Return New Byte() {}
            Dim dummy(maxLen - 1) As Byte
            Dim ln As Integer = FLACGetPictureArrayW(dummy, maxLen, Index)
            If ln <= 0 OrElse ln > maxLen Then Return New Byte() {}
            If ln < maxLen Then Array.Resize(dummy, ln)
            Return dummy
        End Function
        Public Shared Function FLACGetPictureDescription(Index As Short) As String
            Return SafeStr(FLACGetPictureDescriptionW(Index))
        End Function
        Public Shared Function FLACGetPictureFile(FileName As String, Index As Short) As Short
            Return FLACGetPictureFileW(FileName, Index)
        End Function
        Public Shared Function FLACGetPictureMime(Index As Short) As String
            Return SafeStr(FLACGetPictureMimeW(Index))
        End Function
        Public Shared Function FLACGetPictureType(Index As Short) As Short
            Return FLACGetPictureTypeW(Index)
        End Function
        Public Shared Function FLACGetPictureTypeText(Index As Short) As String
            Return SafeStr(FLACGetPictureTypeTextW(Index))
        End Function
        Public Shared Function FLACGetPictureSize(Index As Short) As Integer
            Return FLACGetPictureSizeW(Index)
        End Function
        Public Shared Function FLACGetSamples() As Integer
            Return FLACGetSamplesW()
        End Function
        Public Shared Function FLACGetUserItem(ItemKey As String) As String
            Return SafeStr(FLACGetUserItemW(ItemKey))
        End Function
        Public Shared Function FLACSaveChangesToFile(FileName As String) As Boolean
            Return Convert.ToBoolean(FLACSaveChangesToFileW(FileName))
        End Function
        Public Shared Function FLACSaveChanges() As Boolean
            Return Convert.ToBoolean(FLACSaveChangesW())
        End Function
        Public Shared Sub FLACSetUserItem(ItemKey As String, textString As String)
            FLACSetUserItemW(ItemKey, textString)
        End Sub
#End Region

        Public Shared Function GetMD5ValueFromFile(FileName As String) As String
            Return SafeStr(GetMD5ValueFromFileW(FileName))
        End Function

#Region "ID3V1 Methods"
        Public Shared Function ID3V1Exists() As Boolean
            Return Convert.ToBoolean(ID3V1ExistsW())
        End Function
        Public Shared Function ID3V1GetGenreItem(number As Short) As String
            Return SafeStr(ID3V1GetGenreItemW(number))
        End Function
        Public Shared Function ID3V1GetGenres() As Short
            Return ID3V1GetGenresW()
        End Function
        Public Shared Function ID3V1GetVersion() As String
            Return SafeStr(ID3V1GetVersionW())
        End Function
        Public Shared Function ID3V1RemoveTagFromFile(FileName As String) As Boolean
            Return Convert.ToBoolean(ID3V1RemoveTagFromFileW(FileName))
        End Function
        Public Shared Function ID3V1RemoveTag() As Boolean
            Return Convert.ToBoolean(ID3V1RemoveTagW())
        End Function
        Public Shared Function ID3V1SaveChangesToFile(FileName As String) As Boolean
            Return Convert.ToBoolean(ID3V1SaveChangesToFileW(FileName))
        End Function
        Public Shared Function ID3V1SaveChanges() As Boolean
            Return Convert.ToBoolean(ID3V1SaveChangesW())
        End Function
#End Region

#Region "ID3V2 Methods"
        Public Shared Function ID3V2AddAudioEncryption(arr As Byte(), URL As String, PreviewStart As Short, PreviewLength As Short) As Short
            Return ID3V2AddAudioEncryptionW(arr, arr.Length, URL, PreviewStart, PreviewLength)
        End Function
        Public Shared Function ID3V2AddAudioSeekPoint(arr As Byte(), start As Integer, length As Integer, numbers As Short, BitsPerPoint As Byte) As Short
            Return ID3V2AddAudioSeekPointW(arr, arr.Length, start, length, numbers, BitsPerPoint)
        End Function
        Public Shared Function ID3V2GetSeekOffset() As Integer
            Return ID3V2GetSeekOffsetW()
        End Function
        Public Shared Function ID3V2AddChapter(ID As String, Title As String, Description As String, startTime As Integer, endTime As Integer) As Short
            Return ID3V2AddChapterW(ID, Title, Description, startTime, endTime)
        End Function
        Public Shared Function ID3V2AddChildElement(ParentTocID As String, ChildID As String) As Short
            Return ID3V2AddChildElementW(ParentTocID, ChildID)
        End Function
        Public Shared Function ID3V2AddComment(Language As String, Description As String, Text As String) As Short
            Return ID3V2AddCommentW(Language, Description, Text)
        End Function
        Public Shared Function ID3V2AddCommercialFrame(arr As Byte(), Price As String, validUntil As String, contactUrl As String, receivedAs As Short, seller As String, description As String) As Short
            Return ID3V2AddCommercialFrameW(arr, arr.Length, Price, validUntil, contactUrl, receivedAs, seller, description)
        End Function
        Public Shared Function ID3V2AddEncryption(arr As Byte(), URL As String, Symbol As Short) As Short
            Return ID3V2AddEncryptionW(arr, arr.Length, URL, Symbol)
        End Function
        Public Shared Function ID3V2AddEqualisation(arr As Byte(), Interpolationmethod As Byte, Identification As String) As Short
            Return ID3V2AddEqualisationW(arr, arr.Length, Interpolationmethod, Identification)
        End Function
        Public Shared Function ID3V2AddGeneralObject(arr As Byte(), Mime As String, FileName As String, Description As String) As Short
            Return ID3V2AddGeneralObjectW(arr, arr.Length, Mime, FileName, Description)
        End Function
        Public Shared Function ID3V2AddGroupIdentification(arr As Byte(), Url As String, symbol As Byte) As Short
            Return ID3V2AddGroupIdentificationW(arr, arr.Length, Url, symbol)
        End Function
        Public Shared Function ID3V2AddLinkedInformation(FrameIdentifier As Integer, URL As String, additionalData As String) As Short
            Return ID3V2AddLinkedInformationW(FrameIdentifier, URL, additionalData)
        End Function
        Public Shared Function ID3V2AddLyric(Language As String, Description As String, Text As String) As Short
            Return ID3V2AddLyricW(Language, Description, Text)
        End Function
        Public Shared Function ID3V2AddMpegLocationLookupTable(arr As Byte(), Frames As Integer, Bytes As Integer, Milliseconds As Integer, BytesDeviation As Byte, MillisecondsDeviation As Byte) As Short
            Return ID3V2AddMpegLocationLookupTableW(arr, arr.Length, Frames, Bytes, Milliseconds, BytesDeviation, MillisecondsDeviation)
        End Function
        Public Shared Function ID3V2AddMusicCdIdentifier(arr As Byte()) As Short
            Return ID3V2AddMusicCdIdentifierW(arr, arr.Length)
        End Function
        Public Shared Function ID3V2AddOwnership(Price As String, dateString As String, Seller As String) As Short
            Return ID3V2AddOwnershipW(Price, dateString, Seller)
        End Function
        Public Shared Function ID3V2AddPictureArray(arr As Byte(), Description As String, PictureType As Short) As Short
            Return ID3V2AddPictureArrayW(arr, arr.Length, Description, PictureType)
        End Function
        Public Shared Function ID3V2AddPictureFile(FileName As String, Description As String, PictureType As Short, asLink As Short) As Short
            Return ID3V2AddPictureFileW(FileName, Description, PictureType, asLink)
        End Function
        Public Shared Function ID3V2AddPopularimeter(Email As String, Rating As Short, Counter As Integer) As Short
            Return ID3V2AddPopularimeterW(Email, Rating, Counter)
        End Function
        Public Shared Function ID3V2AddPositionSynchronisation(TimestampFormat As Byte, Position As Integer) As Short
            Return ID3V2AddPositionSynchronisationW(TimestampFormat, Position)
        End Function
        Public Shared Function ID3V2AddPrivateFrame(arr As Byte(), URL As String) As Short
            Return ID3V2AddPrivateFrameW(arr, arr.Length, URL)
        End Function
        Public Shared Function ID3V2AddRecommendedBufferSize(BufferSize As Integer, EmbeddedInfoFlag As Byte, Offset As Integer) As Short
            Return ID3V2AddRecommendedBufferSizeW(BufferSize, EmbeddedInfoFlag, Offset)
        End Function
        Public Shared Function ID3V2AddRelativeVolumeAdjustment(arr As Byte(), Identifier As String) As Short
            Return ID3V2AddRelativeVolumeAdjustmentW(arr, arr.Length, Identifier)
        End Function
        Public Shared Function ID3V2AddReverb(reverbLeft As Short, reverbRight As Short, bouncesLeft As Byte, bouncesRight As Byte, feedbackLeftToLeft As Byte, feedbackLeftToRight As Byte, feedbackRightToRight As Byte, feedbackRightToLeft As Byte, premixLeftToRight As Byte, premixRightToLeft As Byte) As Short
            Return ID3V2AddReverbW(reverbLeft, reverbRight, bouncesLeft, bouncesRight, feedbackLeftToLeft, feedbackLeftToRight, feedbackRightToRight, feedbackRightToLeft, premixLeftToRight, premixRightToLeft)
        End Function
        Public Shared Function ID3V2AddSignatureFrame(arr As Byte(), GroupSymbol As Short) As Short
            Return ID3V2AddSignatureFrameW(arr, arr.Length, GroupSymbol)
        End Function
        Public Shared Function ID3V2AddSyncLyric(Language As String, Description As String, Text As String, ContentType As Short, TimeStampFormat As Short) As Short
            Return ID3V2AddSyncLyricW(Language, Description, Text, ContentType, TimeStampFormat)
        End Function
        Public Shared Function ID3V2AddSynchronizedTempo(arr As Byte(), Format As Short) As Short
            Return ID3V2AddSynchronizedTempoW(arr, arr.Length, Format)
        End Function
        Public Shared Function ID3V2AddTableOfContent(ID As String, Title As String, Description As String, isOrdered As Short) As Short
            Return ID3V2AddTableOfContentW(ID, Title, Description, isOrdered)
        End Function
        Public Shared Function ID3V2AddUniqueFileIdentifier(arr As Byte(), Owner As String) As Short
            Return ID3V2AddUniqueFileIdentifierW(arr, arr.Length, Owner)
        End Function
        Public Shared Function ID3V2AddUserFrame(Language As String, Text As String) As Short
            Return ID3V2AddUserFrameW(Language, Text)
        End Function
        Public Shared Function ID3V2AddUserText(Description As String, Text As String) As Short
            Return ID3V2AddUserTextW(Description, Text)
        End Function
        Public Shared Function ID3V2AddUserURL(Description As String, URL As String) As Short
            Return ID3V2AddUserURLW(Description, URL)
        End Function
        Public Shared Function ID3V2DeleteAddendum(ID As String) As Short
            Return ID3V2DeleteAddendumW(ID)
        End Function
        Public Shared Function ID3V2DeleteAllFrames(FrameID As ID3V2FRAMES) As Short
            Return ID3V2DeleteAllFramesW(FrameID)
        End Function
        Public Shared Function ID3V2DeleteChildElement(ParentTocID As String, ChildID As String) As Short
            Return ID3V2DeleteChildElementW(ParentTocID, ChildID)
        End Function
        Public Shared Function ID3V2DeleteSelectedFrame(FrameID As ID3V2FRAMES, Index As Short) As Short
            Return ID3V2DeleteSelectedFrameW(FrameID, Index)
        End Function
        Public Shared Function ID3V2DeleteSubFrame(ID As String, FrameID As ID3V2FRAMES) As Short
            Return ID3V2DeleteSubFrameW(ID, FrameID)
        End Function
        Public Shared Function ID3V2Exists() As Boolean
            Return Convert.ToBoolean(ID3V2ExistsW())
        End Function
        Public Shared Function ID3V2GetAddendumDescription(ID As String) As String
            Return SafeStr(ID3V2GetAddendumDescriptionW(ID))
        End Function
        Public Shared Function ID3V2GetAddendumTitle(ID As String) As String
            Return SafeStr(ID3V2GetAddendumTitleW(ID))
        End Function
        Public Shared Function ID3V2GetAddendumType(ID As String) As Short
            Return ID3V2GetAddendumTypeW(ID)
        End Function
        Public Shared Function ID3V2GetAllFrameIDs() As String
            Return SafeStr(ID3V2GetAllFrameIDsW())
        End Function
        Public Shared Function ID3V2GetAudioEncryptionData(arr As Byte(), maxLen As Integer, Index As Short) As Integer
            Return ID3V2GetAudioEncryptionDataW(arr, maxLen, Index)
        End Function
        Public Shared Function ID3V2GetAudioEncryptionPreviewLength(Index As Short) As Short
            Return ID3V2GetAudioEncryptionPreviewLengthW(Index)
        End Function
        Public Shared Function ID3V2GetAudioEncryptionPreviewStart(Index As Short) As Short
            Return ID3V2GetAudioEncryptionPreviewStartW(Index)
        End Function
        Public Shared Function ID3V2GetAudioEncryptionURL(Index As Short) As String
            Return SafeStr(ID3V2GetAudioEncryptionURLW(Index))
        End Function
        Public Shared Function ID3V2GetAudioSeekPointBitsPerIndexpoint() As Integer
            Return ID3V2GetAudioSeekPointBitsPerIndexpointW()
        End Function
        Public Shared Function ID3V2GetAudioSeekPointData(arr As Byte(), maxLen As Integer) As Integer
            Return ID3V2GetAudioSeekPointDataW(arr, maxLen)
        End Function
        Public Shared Function ID3V2GetAudioSeekPointLength() As Integer
            Return ID3V2GetAudioSeekPointLengthW()
        End Function
        Public Shared Function ID3V2GetAudioSeekPointNumber() As Integer
            Return ID3V2GetAudioSeekPointNumberW()
        End Function
        Public Shared Function ID3V2GetAudioSeekPointStart() As Integer
            Return ID3V2GetAudioSeekPointStartW()
        End Function
        Public Shared Function ID3v2GetChapterStartTime(ID As String) As Integer
            Return ID3V2GetChapterStartTimeW(ID)
        End Function
        Public Shared Function ID3v2GetChapterEndTime(ID As String) As Integer
            Return ID3V2GetChapterEndTimeW(ID)
        End Function
        Public Shared Function ID3V2GetChildElementID(ID As String, Index As Short) As String
            Return SafeStr(ID3V2GetChildElementIDW(ID, Index))
        End Function
        Public Shared Function ID3V2GetChildElements(ID As String) As Short
            Return ID3V2GetChildElementsW(ID)
        End Function
        Public Shared Function ID3V2GetCommentDescription(Index As Short) As String
            Return SafeStr(ID3V2GetCommentDescriptionW(Index))
        End Function
        Public Shared Function ID3V2GetCommentLanguage(Index As Short) As String
            Return SafeStr(ID3V2GetCommentLanguageW(Index))
        End Function
        Public Shared Function ID3V2GetComment(Index As Short) As String
            Return SafeStr(ID3V2GetCommentW(Index))
        End Function
        Public Shared Function ID3V2GetCommercialFrameContactURL(Index As Short) As String
            Return SafeStr(ID3V2GetCommercialFrameContactURLW(Index))
        End Function
        Public Shared Function ID3V2GetCommercialFrameDescription(Index As Short) As String
            Return SafeStr(ID3V2GetCommercialFrameDescriptionW(Index))
        End Function
        Public Shared Function ID3V2GetCommercialFramePictureMime(Index As Short) As String
            Return SafeStr(ID3V2GetCommercialFramePictureMimeW(Index))
        End Function
        Public Shared Function ID3V2GetCommercialFramePicture(arr As Byte(), maxLen As Integer, Index As Short) As Integer
            Return ID3V2GetCommercialFramePictureW(arr, maxLen, Index)
        End Function
        Public Shared Function ID3V2GetCommercialFramePrice(Index As Short) As String
            Return SafeStr(ID3V2GetCommercialFramePriceW(Index))
        End Function
        Public Shared Function ID3V2GetCommercialFrameReceivedAs(Index As Short) As Short
            Return ID3V2GetCommercialFrameReceivedAsW(Index)
        End Function
        Public Shared Function ID3V2GetCommercialFrameSellerName(Index As Short) As String
            Return SafeStr(ID3V2GetCommercialFrameSellerNameW(Index))
        End Function
        Public Shared Function ID3V2GetCommercialFrameValidUntil(Index As Short) As String
            Return SafeStr(ID3V2GetCommercialFrameValidUntilW(Index))
        End Function
        Public Shared Function ID3V2GetEncoding(FrameID As ID3V2FRAMES) As Short
            Return ID3V2GetEncodingW(FrameID)
        End Function
        Public Shared Function ID3V2GetEncryptionData(arr As Byte(), maxLen As Integer, Index As Short) As Integer
            Return ID3V2GetEncryptionDataW(arr, maxLen, Index)
        End Function
        Public Shared Function ID3V2GetEncryptionSymbol(Index As Short) As Short
            Return ID3V2GetEncryptionSymbolW(Index)
        End Function
        Public Shared Function ID3V2GetEncryptionURL(Index As Short) As String
            Return SafeStr(ID3V2GetEncryptionURLW(Index))
        End Function
        Public Shared Function ID3V2GetEqualisationAdjustmentBits(Index As Short) As Short
            Return ID3V2GetEqualisationAdjustmentBitsW(Index)
        End Function
        Public Shared Function ID3V2GetEqualisationData(arr As Byte(), maxLen As Integer, Index As Short) As Integer
            Return ID3V2GetEqualisationDataW(arr, maxLen, Index)
        End Function
        Public Shared Function ID3V2GetEqualisationIdentification(Index As Short) As String
            Return SafeStr(ID3V2GetEqualisationIdentificationW(Index))
        End Function
        Public Shared Function ID3V2GetEqualisationInterpolation(Index As Short) As Short
            Return ID3V2GetEqualisationInterpolationW(Index)
        End Function
        Public Shared Function ID3V2GetEventTimingCodesData(arr As Byte(), maxLen As Integer) As Integer
            Return ID3V2GetEventTimingCodesDataW(arr, maxLen)
        End Function
        Public Shared Function ID3V2GetEventTimingCodesTimestampFormat() As Short
            Return ID3V2GetEventTimingCodesTimestampFormatW()
        End Function
        Public Shared Function ID3V2GetFrameCount(FrameID As ID3V2FRAMES) As Short
            Return ID3V2GetFrameCountW(FrameID)
        End Function
        Public Shared Function ID3V2GetGeneralObjectData(arr As Byte(), maxLen As Integer, Index As Short) As Integer
            Return ID3V2GetGeneralObjectDataW(arr, maxLen, Index)
        End Function
        Public Shared Function ID3V2GetGeneralObjectDescription(Index As Short) As String
            Return SafeStr(ID3V2GetGeneralObjectDescriptionW(Index))
        End Function
        Public Shared Function ID3V2GetGeneralObjectFilename(Index As Short) As String
            Return SafeStr(ID3V2GetGeneralObjectFilenameW(Index))
        End Function
        Public Shared Function ID3V2GetGeneralObjectMime(Index As Short) As String
            Return SafeStr(ID3V2GetGeneralObjectMimeW(Index))
        End Function
        Public Shared Function ID3V2GetGenre() As String
            Return SafeStr(ID3V2GetGenreW())
        End Function
        Public Shared Function ID3V2GetGroupIdentificationData(arr As Byte(), maxLen As Integer, Index As Short) As Integer
            Return ID3V2GetGroupIdentificationDataW(arr, maxLen, Index)
        End Function
        Public Shared Function ID3V2GetGroupIdentificationSymbol(Index As Short) As Short
            Return ID3V2GetGroupIdentificationSymbolW(Index)
        End Function
        Public Shared Function ID3V2GetGroupIdentificationURL(Index As Short) As String
            Return SafeStr(ID3V2GetGroupIdentificationURLW(Index))
        End Function
        Public Shared Function ID3V2GetLinkedInformationAdditionalData(Index As Short) As String
            Return SafeStr(ID3V2GetLinkedInformationAdditionalDataW(Index))
        End Function
        Public Shared Function ID3V2GetLinkedInformationFrameIdentifier(Index As Short) As Integer
            Return ID3V2GetLinkedInformationFrameIdentifierW(Index)
        End Function
        Public Shared Function ID3V2GetLinkedInformationURL(Index As Short) As String
            Return SafeStr(ID3V2GetLinkedInformationURLW(Index))
        End Function
        Public Shared Function ID3V2GetLyricDescription(Index As Short) As String
            Return SafeStr(ID3V2GetLyricDescriptionW(Index))
        End Function
        Public Shared Function ID3V2GetLyricLanguage(Index As Short) As String
            Return SafeStr(ID3V2GetLyricLanguageW(Index))
        End Function
        Public Shared Function ID3V2GetLyric(Index As Short) As String
            Return SafeStr(ID3V2GetLyricW(Index))
        End Function
        Public Shared Function ID3V2GetMpegLocationLookupTableBytesDeviation() As Integer
            Return ID3V2GetMpegLocationLookupTableBytesDeviationW()
        End Function
        Public Shared Function ID3V2GetMpegLocationLookupTableBytes() As Integer
            Return ID3V2GetMpegLocationLookupTableBytesW()
        End Function
        Public Shared Function ID3V2GetMpegLocationLookupTableData(arr As Byte(), maxLen As Integer) As Integer
            Return ID3V2GetMpegLocationLookupTableDataW(arr, maxLen)
        End Function
        Public Shared Function ID3V2GetMpegLocationLookupTableFrames() As Integer
            Return ID3V2GetMpegLocationLookupTableFramesW()
        End Function
        Public Shared Function ID3V2GetMpegLocationLookupTableMillisecondsDeviation() As Integer
            Return ID3V2GetMpegLocationLookupTableMillisecondsDeviationW()
        End Function
        Public Shared Function ID3V2GetMpegLocationLookupTableMilliseconds() As Integer
            Return ID3V2GetMpegLocationLookupTableMillisecondsW()
        End Function
        Public Shared Function ID3V2GetMusicCdIdentifierData(arr As Byte(), maxLen As Integer) As Integer
            Return ID3V2GetMusicCdIdentifierDataW(arr, maxLen)
        End Function
        Public Shared Function ID3V2GetOwnershipDate() As String
            Return SafeStr(ID3V2GetOwnershipDateW())
        End Function
        Public Shared Function ID3V2GetOwnershipPrice() As String
            Return SafeStr(ID3V2GetOwnershipPriceW())
        End Function
        Public Shared Function ID3V2GetOwnershipSeller() As String
            Return SafeStr(ID3V2GetOwnershipSellerW())
        End Function
        Public Shared Function ID3V2GetPictureCount() As Short
            Return ID3V2GetFrameCount(ID3V2FRAMES.ID3F_APIC)
        End Function
        Public Shared Function ID3V2GetPictureArray(Index As Short) As Byte()
            Dim maxLen As Integer = ID3V2GetPictureSize(Index)
            If maxLen <= 0 Then Return New Byte() {}
            Dim dummy(maxLen - 1) As Byte
            Dim ln As Integer = ID3V2GetPictureArrayW(dummy, maxLen, Index)
            If ln <= 0 OrElse ln > maxLen Then Return New Byte() {}
            If ln < maxLen Then Array.Resize(dummy, ln)
            Return dummy
        End Function
        Public Shared Function ID3V2GetPictureDescription(Index As Short) As String
            Return SafeStr(ID3V2GetPictureDescriptionW(Index))
        End Function
        Public Shared Function ID3V2GetPictureFile(FileName As String, Index As Short) As Short
            Return ID3V2GetPictureFileW(FileName, Index)
        End Function
        Public Shared Function ID3V2GetPictureMime(Index As Short) As String
            Return SafeStr(ID3V2GetPictureMimeW(Index))
        End Function
        Public Shared Function ID3V2GetPictureType(Index As Short) As Short
            Return ID3V2GetPictureTypeW(Index)
        End Function
        Public Shared Function ID3V2GetPictureTypeText(Index As Short) As String
            Return SafeStr(ID3V2GetPictureTypeTextW(Index))
        End Function
        Public Shared Function ID3V2GetPictureSize(Index As Short) As Integer
            Return ID3V2GetPictureSizeW(Index)
        End Function
        Public Shared Function ID3V2GetPlayCounter() As Integer
            Return ID3V2GetPlayCounterW()
        End Function
        Public Shared Function ID3V2GetPopularimeterCounter(Index As Short) As Integer
            Return ID3V2GetPopularimeterCounterW(Index)
        End Function
        Public Shared Function ID3V2GetPopularimeterEmail(Index As Short) As String
            Return SafeStr(ID3V2GetPopularimeterEmailW(Index))
        End Function
        Public Shared Function ID3V2GetPopularimeterRating(Index As Short) As Short
            Return ID3V2GetPopularimeterRatingW(Index)
        End Function
        Public Shared Function ID3V2GetPositionSynchronisationTimestampFormat() As Short
            Return ID3V2GetPositionSynchronisationTimestampFormatW()
        End Function
        Public Shared Function ID3V2GetPositionSynchronisationValue() As Integer
            Return ID3V2GetPositionSynchronisationValueW()
        End Function
        Public Shared Function ID3V2GetPossibleCHAPID() As String
            Return SafeStr(ID3V2GetPossibleCHAPIDW())
        End Function
        Public Shared Function ID3V2GetPossibleTOCID() As String
            Return SafeStr(ID3V2GetPossibleTOCIDW())
        End Function
        Public Shared Function ID3V2GetPrivateFrameData(arr As Byte(), maxLen As Integer, Index As Short) As Integer
            Return ID3V2GetPrivateFrameDataW(arr, maxLen, Index)
        End Function
        Public Shared Function ID3V2GetPrivateFrameURL(Index As Short) As String
            Return SafeStr(ID3V2GetPrivateFrameURLW(Index))
        End Function
        Public Shared Function ID3V2GetRelativeVolumeAdjustmentData(arr As Byte(), maxLen As Integer, Index As Short) As Integer
            Return ID3V2GetRelativeVolumeAdjustmentDataW(arr, maxLen, Index)
        End Function
        Public Shared Function ID3V2GetRelativeVolumeAdjustmentIdentifier(Index As Short) As String
            Return SafeStr(ID3V2GetRelativeVolumeAdjustmentIdentifierW(Index))
        End Function
        Public Shared Function ID3V2GetReverbBouncesLeft() As Short
            Return ID3V2GetReverbBouncesLeftW()
        End Function
        Public Shared Function ID3V2GetReverbBouncesRight() As Short
            Return ID3V2GetReverbBouncesRightW()
        End Function
        Public Shared Function ID3V2GetReverbFeedbackLeftToLeft() As Short
            Return ID3V2GetReverbFeedbackLeftToLeftW()
        End Function
        Public Shared Function ID3V2GetReverbFeedbackLeftToRight() As Short
            Return ID3V2GetReverbFeedbackLeftToRightW()
        End Function
        Public Shared Function ID3V2GetReverbFeedbackRightToLeft() As Short
            Return ID3V2GetReverbFeedbackRightToLeftW()
        End Function
        Public Shared Function ID3V2GetReverbFeedbackRightToRight() As Short
            Return ID3V2GetReverbFeedbackRightToRightW()
        End Function
        Public Shared Function ID3V2GetReverbLeft() As Short
            Return ID3V2GetReverbLeftW()
        End Function
        Public Shared Function ID3V2GetReverbPremixLeftToRight() As Short
            Return ID3V2GetReverbPremixLeftToRightW()
        End Function
        Public Shared Function ID3V2GetReverbPremixRightToLeft() As Short
            Return ID3V2GetReverbPremixRightToLeftW()
        End Function
        Public Shared Function ID3V2GetReverbRight() As Short
            Return ID3V2GetReverbRightW()
        End Function
        Public Shared Function ID3V2GetSignatureFrameData(arr As Byte(), maxLen As Integer, Index As Short) As Integer
            Return ID3V2GetSignatureFrameDataW(arr, maxLen, Index)
        End Function
        Public Shared Function ID3V2GetSignatureFrameGroupSymbol(Index As Short) As Short
            Return ID3V2GetSignatureFrameGroupSymbolW(Index)
        End Function
        Public Shared Function ID3V2GetSize() As Integer
            Return ID3V2GetSizeW()
        End Function
        Public Shared Function ID3V2GetSubFrameDescription(ID As String, Index As Short) As String
            Return SafeStr(ID3V2GetSubFrameDescriptionW(ID, Index))
        End Function
        Public Shared Function ID3V2GetSubFrameID(ID As String, Index As Short) As Integer
            Return ID3V2GetSubFrameIDW(ID, Index)
        End Function
        Public Shared Function ID3V2GetSubFrameImageType(ID As String, Index As Short) As Short
            Return ID3V2GetSubFrameImageTypeW(ID, Index)
        End Function
        Public Shared Function ID3V2GetSubFrameImage(ID As String, Index As Short) As Byte()
            Dim maxLen As Integer = 1000000
            Dim dummy(maxLen - 1) As Byte
            Dim ln As Integer = ID3V2GetSubFrameImageW(dummy, maxLen, ID, Index)
            If ln <= 0 OrElse ln > maxLen Then Return New Byte() {}
            If ln < maxLen Then Array.Resize(dummy, ln)
            Return dummy
        End Function
        Public Shared Function ID3V2GetSubFrameText(ID As String, Index As Short) As String
            Return SafeStr(ID3V2GetSubFrameTextW(ID, Index))
        End Function
        Public Shared Function ID3V2GetSubFrameType(ID As String, Index As Short) As Short
            Return ID3V2GetSubFrameTypeW(ID, Index)
        End Function
        Public Shared Function ID3V2GetSubFrames(ID As String) As Short
            Return ID3V2GetSubFramesW(ID)
        End Function
        Public Shared Function ID3V2GetSyncLyricContentType(Index As Short) As Short
            Return ID3V2GetSyncLyricContentTypeW(Index)
        End Function
        Public Shared Function ID3V2GetSyncLyricDescription(Index As Short) As String
            Return SafeStr(ID3V2GetSyncLyricDescriptionW(Index))
        End Function
        Public Shared Function ID3V2GetSyncLyricLanguage(Index As Short) As String
            Return SafeStr(ID3V2GetSyncLyricLanguageW(Index))
        End Function
        Public Shared Function ID3V2GetSyncLyricTimeFormat(Index As Short) As Short
            Return ID3V2GetSyncLyricTimeFormatW(Index)
        End Function
        Public Shared Function ID3V2GetSyncLyric(Index As Short) As String
            Return SafeStr(ID3V2GetSyncLyricW(Index))
        End Function
        Public Shared Function ID3V2GetSynchronizedTempoData(arr As Byte(), maxLen As Integer) As Integer
            Return ID3V2GetSynchronizedTempoDataW(arr, maxLen)
        End Function
        Public Shared Function ID3V2GetSynchronizedTempoFormat() As Short
            Return ID3V2GetSynchronizedTempoFormatW()
        End Function
        Public Shared Function ID3V2GetTOCRootID() As String
            Return SafeStr(ID3V2GetTOCRootIDW())
        End Function
        Public Shared Function ID3V2GetTextFrame(FrameID As ID3V2FRAMES) As String
            Return SafeStr(ID3V2GetTextFrameW(FrameID))
        End Function
        Public Shared Function ID3V2GetURLFrame(FrameID As ID3V2FRAMES) As String
            Return SafeStr(ID3V2GetURLFrameW(FrameID))
        End Function
        Public Shared Function ID3V2GetUniqueFileIdentifierData(arr As Byte(), maxLen As Integer, Index As Short) As Integer
            Return ID3V2GetUniqueFileIdentifierDataW(arr, maxLen, Index)
        End Function
        Public Shared Function ID3V2GetUniqueFileIdentifierOwner(Index As Short) As String
            Return SafeStr(ID3V2GetUniqueFileIdentifierOwnerW(Index))
        End Function
        Public Shared Function ID3V2GetUserFrameLanguage(Index As Short) As String
            Return SafeStr(ID3V2GetUserFrameLanguageW(Index))
        End Function
        Public Shared Function ID3V2GetUserFrame(Index As Short) As String
            Return SafeStr(ID3V2GetUserFrameW(Index))
        End Function
        Public Shared Function ID3V2GetUserTextDescription(Index As Short) As String
            Return SafeStr(ID3V2GetUserTextDescriptionW(Index))
        End Function
        Public Shared Function ID3V2GetUserText(Index As Short) As String
            Return SafeStr(ID3V2GetUserTextW(Index))
        End Function
        Public Shared Function ID3V2GetUserURLDescription(Index As Short) As String
            Return SafeStr(ID3V2GetUserURLDescriptionW(Index))
        End Function
        Public Shared Function ID3V2GetUserURL(Index As Short) As String
            Return SafeStr(ID3V2GetUserURLW(Index))
        End Function
        Public Shared Function ID3V2GetVersion() As String
            Return SafeStr(ID3V2GetVersionW())
        End Function
        Public Shared Function ID3V2GetTOCIsOrdered(ID As String) As Boolean
            Return Convert.ToBoolean(ID3V2GetTOCIsOrderedW(ID))
        End Function
        Public Shared Function ID3V2GetRecommendedBufferSizeFlag() As Short
            Return ID3V2GetRecommendedBufferSizeFlagW()
        End Function
        Public Shared Function ID3V2RecommendedBufferSizeOffset() As Integer
            Return ID3V2GetRecommendedBufferSizeOffsetW()
        End Function
        Public Shared Function ID3V2GetRecommendedBufferSizeValue() As Integer
            Return ID3V2GetRecommendedBufferSizeValueW()
        End Function
        Public Shared Function ID3V2RemoveTagFromFile(FileName As String) As Short
            Return ID3V2RemoveTagFromFileW(FileName)
        End Function
        Public Shared Function ID3V2RemoveTag() As Short
            Return ID3V2RemoveTagW()
        End Function
        Public Shared Function ID3V2AddEventTimingCodes(arr As Byte(), maxLen As Integer, TimestampFormat As Byte) As Short
            Return ID3V2AddEventTimingCodesW(arr, maxLen, TimestampFormat)
        End Function
        Public Shared Function ID3V2SaveChangesToFile(FileName As String) As Short
            Return ID3V2SaveChangesToFileW(FileName)
        End Function
        Public Shared Function ID3V2SaveChanges() As Short
            Return ID3V2SaveChangesW()
        End Function
        Public Shared Function ID3v2SetAddendumDescription(ID As String, newDescription As String) As Integer
            Return ID3V2SetAddendumDescriptionW(ID, newDescription)
        End Function
        Public Shared Function ID3v2SetAddendumTitle(ID As String, newTitle As String) As Integer
            Return ID3V2SetAddendumTitleW(ID, newTitle)
        End Function
        Public Shared Sub ID3V2SetChapterTimes(ID As String, startTime As Integer, endTime As Integer)
            ID3V2SetChapterTimesW(ID, startTime, endTime)
        End Sub
        Public Shared Function ID3V2SetFormatAndEncoding(format As Short, encoding As Short) As Short
            Return ID3V2SetFormatAndEncodingW(format, encoding)
        End Function
        Public Shared Sub ID3V2SetGenre(textString As String)
            ID3V2SetGenreW(textString)
        End Sub
        Public Shared Function ID3V2SetImageSubFrame(arr As Byte(), ID As String, Description As String, pictureType As PictureType) As Short
            Return ID3V2SetImageSubFrameW(arr, arr.Length, ID, Description, pictureType)
        End Function
        Public Shared Sub ID3V2SetPlayCounter(counter As Integer)
            ID3V2SetPlayCounterW(counter)
        End Sub
        Public Shared Sub ID3V2AddSeekOffset(offset As Integer)
            ID3V2AddSeekOffsetW(offset)
        End Sub
        Public Shared Sub ID3V2SetTextFrame(FrameID As ID3V2FRAMES, textString As String)
            ID3V2SetTextFrameW(FrameID, textString)
        End Sub
        Public Shared Function ID3V2SetTextSubFrame(ID As String, FrameID As ID3V2FRAMES, textString As String, description As String) As Short
            Return ID3V2SetTextSubFrameW(ID, FrameID, textString, description)
        End Function
        Public Shared Sub ID3V2SetURLFrame(FrameID As ID3V2FRAMES, textString As String)
            ID3V2SetURLFrameW(FrameID, textString)
        End Sub
        Public Shared Function ID3V2SetURLSubFrame(ID As String, FrameID As ID3V2FRAMES, urlString As String, description As String) As Short
            Return ID3V2SetURLSubFrameW(ID, FrameID, urlString, description)
        End Function
        Public Shared Function ID3V2SetTOCIsOrdered(ID As String, isOrdered As Boolean) As Short
            Return ID3V2SetTOCIsOrderedW(ID, Convert.ToInt16(isOrdered))
        End Function
        Public Shared Function ID3V2ImportCueFile(FileName As String) As Short
            Return ID3V2ImportCueFileW(FileName)
        End Function
#End Region

#Region "LYRICS Methods"
        Public Shared Function LYRICSExists() As Boolean
            Return Convert.ToBoolean(LYRICSExistsW())
        End Function
        Public Shared Function LYRICSGetSize() As Integer
            Return LYRICSGetSizeW()
        End Function
        Public Shared Function LYRICSGetStartPosition() As Integer
            Return LYRICSGetStartPositionW()
        End Function
        Public Shared Function LYRICSGetVersion() As String
            Return SafeStr(LYRICSGetVersionW())
        End Function
        Public Shared Function LYRICSRemoveTagFromFile(FileName As String) As Boolean
            Return Convert.ToBoolean(LYRICSRemoveTagFromFileW(FileName))
        End Function
        Public Shared Function LYRICSRemoveTag() As Boolean
            Return Convert.ToBoolean(LYRICSRemoveTagW())
        End Function
        Public Shared Function LYRICSSaveChangesToFile(FileName As String) As Boolean
            Return Convert.ToBoolean(LYRICSSaveChangesToFileW(FileName))
        End Function
        Public Shared Function LYRICSSaveChanges() As Boolean
            Return Convert.ToBoolean(LYRICSSaveChangesW())
        End Function
#End Region

#Region "MONKEY Methods"
        Public Shared Function MONKEYGetBits() As Short
            Return MONKEYGetBitsW()
        End Function
        Public Shared Function MONKEYGetCompressionRatio() As Single
            Return MONKEYGetCompressionRatioW()
        End Function
        Public Shared Function MONKEYGetCompression() As String
            Return SafeStr(MONKEYGetCompressionW())
        End Function
        Public Shared Function MONKEYGetFrames() As Integer
            Return MONKEYGetFramesW()
        End Function
        Public Shared Function MONKEYGetPeak() As Single
            Return MONKEYGetPeakW()
        End Function
        Public Shared Function MONKEYGetSamplesPerFrame() As Integer
            Return MONKEYGetSamplesPerFrameW()
        End Function
        Public Shared Function MONKEYGetSamples() As Integer
            Return MONKEYGetSamplesW()
        End Function
#End Region

#Region "MP4 Methods"
        Public Shared Sub MP4DeleteEntries()
            MP4DeleteEntriesW()
        End Sub
        Public Shared Sub MP4DeletePicture(Index As Short)
            MP4DeletePictureW(Index)
        End Sub
        Public Shared Sub MP4DeletePictures()
            MP4DeletePicturesW()
        End Sub
        Public Shared Function MP4GetAllFrameIDs() As String
            Return SafeStr(MP4GetAllFrameIDsW())
        End Function
        Public Shared Function MP4GetiTuneFrame(FrameID As String) As String
            Return SafeStr(MP4GetiTuneFrameW(FrameID))
        End Function
        Public Shared Function MP4GetTextFrame(FrameID As MP4FRAMES) As String
            Return SafeStr(MP4GetTextFrameW(FrameID))
        End Function
        Public Shared Function MP4GetPictureArray(Index As Short) As Byte()
            Dim maxLen As Integer = MP4GetPictureSize(Index)
            If maxLen <= 0 Then Return New Byte() {}
            Dim dummy(maxLen - 1) As Byte
            Dim ln As Integer = MP4GetPictureArrayW(dummy, maxLen, Index)
            If ln <= 0 OrElse ln > maxLen Then Return New Byte() {}
            If ln < maxLen Then Array.Resize(dummy, ln)
            Return dummy
        End Function
        Public Shared Function MP4GetPictureCount() As Integer
            Return MP4GetPictureCountW()
        End Function
        Public Shared Function MP4GetPictureFile(FileName As String, Index As Short) As Short
            Return MP4GetPictureFileW(FileName, Index)
        End Function
        Public Shared Function MP4GetPictureMime(Index As Short) As String
            Return SafeStr(MP4GetPictureMimeW(Index))
        End Function
        Public Shared Function MP4GetPictureSize(Index As Short) As Integer
            Return MP4GetPictureSizeW(Index)
        End Function
        Public Shared Function MP4GetVersion() As String
            Return SafeStr(MP4GetVersionW())
        End Function
        Public Shared Function MP4SaveChangesToFile(FileName As String) As Boolean
            Return Convert.ToBoolean(MP4SaveChangesToFileW(FileName))
        End Function
        Public Shared Function MP4SaveChanges() As Boolean
            Return Convert.ToBoolean(MP4SaveChangesW())
        End Function
        Public Shared Function MP4AddPictureArray(arr As Byte()) As Short
            Return MP4AddPictureArrayW(arr, arr.Length)
        End Function
        Public Shared Function MP4AddPictureFile(FileName As String) As Short
            Return MP4AddPictureFileW(FileName)
        End Function
        Public Shared Sub MP4SetiTuneFrame(FrameID As String, textString As String)
            MP4SetiTuneFrameW(FrameID, textString)
        End Sub
        Public Shared Sub MP4SetTextFrame(FrameID As MP4FRAMES, textString As String)
            MP4SetTextFrameW(FrameID, textString)
        End Sub
#End Region

#Region "MPEG Methods"
        Public Shared Function MPEGGetEmphasis() As String
            Return SafeStr(MPEGGetEmphasisW())
        End Function
        Public Shared Function MPEGGetEncoder() As String
            Return SafeStr(MPEGGetEncoderW())
        End Function
        Public Shared Function MPEGGetFramePosition() As Integer
            Return MPEGGetFramePositionW()
        End Function
        Public Shared Function MPEGGetFrameSize() As Integer
            Return MPEGGetFrameSizeW()
        End Function
        Public Shared Function MPEGGetFrames() As Integer
            Return MPEGGetFramesW()
        End Function
        Public Shared Function MPEGGetLayer() As String
            Return SafeStr(MPEGGetLayerW())
        End Function
        Public Shared Function MPEGGetVersion() As String
            Return SafeStr(MPEGGetVersionW())
        End Function
        Public Shared Function MPEGGetIsCopyrighted() As Boolean
            Return Convert.ToBoolean(MPEGIsCopyrightedW())
        End Function
        Public Shared Function MPEGIsOriginal() As Boolean
            Return Convert.ToBoolean(MPEGIsOriginalW())
        End Function
        Public Shared Function MPEGIsPadding() As Boolean
            Return Convert.ToBoolean(MPEGIsPaddingW())
        End Function
        Public Shared Function MPEGIsPrivate() As Boolean
            Return Convert.ToBoolean(MPEGIsPrivateW())
        End Function
        Public Shared Function MPEGIsProtected() As Boolean
            Return Convert.ToBoolean(MPEGIsProtectedW())
        End Function
        Public Shared Function MPEGIsVBR() As Boolean
            Return Convert.ToBoolean(MPEGIsVBRW())
        End Function
        Public Shared Function MPEGSetCopyrighted(FileName As String, newValue As Short) As Boolean
            Return Convert.ToBoolean(MPEGSetCopyrightedW(FileName, newValue))
        End Function
        Public Shared Function MPEGSetOriginal(FileName As String, newValue As Short) As Boolean
            Return Convert.ToBoolean(MPEGSetOriginalW(FileName, newValue))
        End Function
        Public Shared Function MPEGSetPrivate(FileName As String, newValue As Short) As Boolean
            Return Convert.ToBoolean(MPEGSetPrivateW(FileName, newValue))
        End Function
#End Region

#Region "MPP Methods"
        Public Shared Function MPPGetFrames() As Integer
            Return MPPGetFramesW()
        End Function
        Public Shared Function MPPGetStreamVersion() As Short
            Return MPPGetStreamVersionW()
        End Function
#End Region

#Region "OGG Methods"
        Public Shared Function OGGGetBitRateNominal() As Integer
            Return OGGGetBitRateNominalW()
        End Function
        Public Shared Function OGGGetItemKeys() As String
            Return SafeStr(OGGGetItemKeysW())
        End Function
        Public Shared Function OGGGetUserItem(ItemKey As String) As String
            Return SafeStr(OGGGetUserItemW(ItemKey))
        End Function
        Public Shared Function OGGRemoveTagFromFile(FileName As String) As Short
            Return OGGRemoveTagFromFileW(FileName)
        End Function
        Public Shared Function OGGRemoveTag() As Boolean
            Return Convert.ToBoolean(OGGRemoveTagW())
        End Function
        Public Shared Function OGGSaveChangesToFile(FileName As String) As Boolean
            Return Convert.ToBoolean(OGGSaveChangesToFileW(FileName))
        End Function
        Public Shared Function OGGSaveChanges() As Boolean
            Return Convert.ToBoolean(OGGSaveChangesW())
        End Function
        Public Shared Sub OGGSetUserItem(ItemKey As String, textString As String)
            OGGSetUserItemW(ItemKey, textString)
        End Sub
#End Region

#Region "WAV Methods"
        Public Shared Function WAVGetBitsPerSample() As Integer
            Return WAVGetBitsPerSampleW()
        End Function
        Public Shared Function WAVGetBlockAlign() As Integer
            Return WAVGetBlockAlignW()
        End Function
        Public Shared Function WAVGetBytesPerSecond() As Integer
            Return WAVGetBytesPerSecondW()
        End Function
        Public Shared Function WAVGetChannels() As Integer
            Return WAVGetChannelsW()
        End Function
        Public Shared Function WAVGetFormatID() As Short
            Return WAVGetFormatIDW()
        End Function
        Public Shared Function WAVGetFormat() As String
            Return SafeStr(WAVGetFormatW())
        End Function
        Public Shared Function WAVGetHeaderSize() As Integer
            Return WAVGetHeaderSizeW()
        End Function
        Public Shared Function WAVGetInfoChunkIDs() As String
            Return SafeStr(WAVGetInfoChunkIDsW())
        End Function
        Public Shared Function WAVGetTextFrame(FrameID As WAVFRAMES) As String
            Return SafeStr(WAVGetTextFrameW(FrameID))
        End Function
        Public Shared Sub WAVSetTextFrame(FrameID As WAVFRAMES, textString As String)
            WAVSetTextFrameW(FrameID, textString)
        End Sub
        Public Shared Function WAVGetCartChunkEntry(Index As WAVCARTENTRIES) As String
            Return SafeStr(WAVGetCartChunkEntryW(CShort(Index)))
        End Function
        Public Shared Sub WAVSetCartChunkEntry(Index As WAVCARTENTRIES, textString As String)
            WAVSetCartChunkEntryW(CShort(Index), textString)
        End Sub
        Public Shared Function WAVSaveChanges() As Boolean
            Return Convert.ToBoolean(WAVSaveChangesW())
        End Function
        Public Shared Function WAVSaveChangesToFile(FileName As String) As Boolean
            Return Convert.ToBoolean(WAVSaveChangesToFileW(FileName))
        End Function
#End Region

#Region "WMA Methods"
        Public Shared Function WMADeletePicture(Index As Short) As Boolean
            Return Convert.ToBoolean(WMADeletePictureW(Index))
        End Function
        Public Shared Sub WMADeletePictures()
            WMADeletePicturesW()
        End Sub
        Public Shared Function WMAGetItemKeys() As String
            Return SafeStr(WMAGetItemKeysW())
        End Function
        Public Shared Function WMAGetPictureArray(Index As Short) As Byte()
            Dim maxLen As Integer = CInt(WMAGetPictureSize(Index))
            If maxLen <= 0 Then Return New Byte() {}
            Dim dummy(maxLen - 1) As Byte
            Dim ln As Integer = WMAGetPictureArrayW(dummy, maxLen, Index)
            If ln <= 0 OrElse ln > maxLen Then Return New Byte() {}
            If ln < maxLen Then Array.Resize(dummy, ln)
            Return dummy
        End Function
        Public Shared Function WMAGetPictureDescription(Index As Short) As String
            Return SafeStr(WMAGetPictureDescriptionW(Index))
        End Function
        Public Shared Function WMAGetPictureMime(Index As Short) As String
            Return SafeStr(WMAGetPictureMimeW(Index))
        End Function
        Public Shared Function WMAGetPictureSize(Index As Short) As Integer
            Return WMAGetPictureSizeW(Index)
        End Function
        Public Shared Function WMAGetPictureType(Index As Short) As Short
            Return WMAGetPictureTypeW(Index)
        End Function
        Public Shared Function WMAGetPictureFile(FileName As String, Index As Short) As Short
            Return WMAGetPictureFileW(FileName, Index)
        End Function
        Public Shared Function WMAGetPictureCount() As Short
            Return WMAGetPictureCountW()
        End Function
        Public Shared Function WMAGetUserItem(ItemKey As String) As String
            Return SafeStr(WMAGetUserItemW(ItemKey))
        End Function
        Public Shared Function WMAIsVBR() As Boolean
            Return Convert.ToBoolean(WMAIsVBRW())
        End Function
        Public Shared Function WMASaveChangesToFile(FileName As String) As Boolean
            Return Convert.ToBoolean(WMASaveChangesToFileW(FileName))
        End Function
        Public Shared Function WMASaveChanges() As Boolean
            Return Convert.ToBoolean(WMASaveChangesW())
        End Function
        Public Shared Function WMAAddPictureArray(arr As Byte(), Description As String, PictureType As Short, Index As Short) As Short
            Return WMAAddPictureArrayW(arr, arr.Length, Description, PictureType, Index)
        End Function
        Public Shared Function WMAAddPictureFile(FileName As String, Description As String, PictureType As Short, Index As Short) As Short
            Return WMAAddPictureFileW(FileName, Description, PictureType, Index)
        End Function
        Public Shared Sub WMASetUserItem(ItemKey As String, textString As String)
            WMASetUserItemW(ItemKey, textString)
        End Sub
#End Region

    End Class
End Namespace

