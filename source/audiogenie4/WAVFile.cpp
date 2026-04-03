/* AudioGenie is a Library for analyzing and tagging audio files.
   Copyright (C) 2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011
   Free Software Foundation, Inc.
   This file is part of the AudioGenie Library.
   Contributed by Stefan Toengi <stefan@toengi.de>.

   The AudioGenie Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see <http://www.gnu.org/licenses/> 
*/

#include "stdafx.h"
#include "stdio.h"
#include "io.h"
#include <fcntl.h>
#include "wavfile.h"
#include "Blob.h"
#include <share.h>
#include "wavformatchunk.h"
#include "wavdatachunk.h"
#include "wavcartchunk.h"
#include "WAVDISPChunk.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CWAVFile::CWAVFile()
{
	mainContainer = new CWAVRIFFContainer();
}

CWAVFile::~CWAVFile()
{
	mainContainer->Remove();
	delete mainContainer;
}

/* -------------------------------------------------------------------------- */

bool CWAVFile::ReadWAV(FILE *Stream)
{
	return mainContainer->load(Stream, CTools::ID3v2Size, CTools::FileSize - CTools::ID3v1Size);
}

/* -------------------------------------------------------------------------- */

bool CWAVFile::HeaderIsValid()
{
	return (mainContainer->getSize() > 20);	
}

/* -------------------------------------------------------------------------- */

void CWAVFile::ResetData()
{
	/* Reset all data */
	mainContainer->Remove();
}

/* -------------------------------------------------------------------------- */

CAtlString CWAVFile::GetFormat()
{
	/* Get format type name */
	CWAVFormatChunk *fmt = mainContainer->getFormatChunk();
	if (fmt == NULL)
		return UNKNOWN;
	switch (fmt->FormatID)
	{
	case 0x0000: return _T("Microsoft Unknown Wave Format");
	case 0x0001: return _T("Microsoft PCM Format");
	case 0x0002: return _T("Microsoft ADPCM Format");
	case 0x0003: return _T("IEEE Float");
	case 0x0004: return _T("Compaq Computer VSELP (codec for Windows CE 2.0 devices)");
	case 0x0005: return _T("IBM CVSD");
	case 0x0006: return _T("Microsoft ALAW (CCITT A-Law)");
	case 0x0007: return _T("Microsoft MULAW (CCITT u-Law)");
	case 0x0008: return _T("Microsoft DTS");
	case 0x0009: return _T("Microsoft DRM");
	case 0x000A: return _T("Microsoft WMSpeech");
	case 0x000B: return _T("Microsoft Windows Media RT Voice");
	case 0x0010: return _T("OKI ADPCM");
	case 0x0011: return _T("Intel DVI ADPCM (IMA ADPCM)");
	case 0x0012: return _T("Videologic MediaSpace ADPCM");
	case 0x0013: return _T("Sierra Semiconductor ADPCM");
	case 0x0014: return _T("Antex Electronics G.723 ADPCM");
	case 0x0015: return _T("DSP Solutions DigiSTD");
	case 0x0016: return _T("DSP Solutions DigiFIX");
	case 0x0017: return _T("Dialogic OKI ADPCM");
	case 0x0018: return _T("MediaVision ADPCM");
	case 0x0019: return _T("Hewlett-Packard CU codec");
	case 0x001A: return _T("Hewlett-Packard HP DYNAMIC VOICE");
	case 0x0020: return _T("Yamaha ADPCM");
	case 0x0021: return _T("Speech Compression SONARC");
	case 0x0022: return _T("DSP Group True Speech");
	case 0x0023: return _T("Echo Speech EchoSC1");
	case 0x0024: return _T("Audiofile AF36");
	case 0x0025: return _T("Audio Processing Technology APTX");
	case 0x0026: return _T("AudioFile AF10");
	case 0x0027: return _T("Prosody 1612 codec for CTI Speech Card");
	case 0x0028: return _T("Merging Technologies S.A. LRC");
	case 0x0030: return _T("Dolby Labs AC2");
	case 0x0031: return _T("Microsoft GSM 6.10");
	case 0x0032: return _T("MSNAudio");
	case 0x0033: return _T("Antex Electronics ADPCME");
	case 0x0034: return _T("Control Resources VQLPC");
	case 0x0035: return _T("DSP Solutions DigiREAL");
	case 0x0036: return _T("DSP Solutions DigiADPCM");
	case 0x0037: return _T("Control Resources CR10");
	case 0x0038: return _T("Natural MicroSystems VBXADPCM");
	case 0x0039: return _T("Roland RDAC (Crystal Semiconductor IMA ADPCM)");
	case 0x003A: return _T("Echo Speech EchoSC3");
	case 0x003B: return _T("Rockwell ADPCM");
	case 0x003C: return _T("Rockwell Digit LK");
	case 0x003D: return _T("Xebec Multimedia Solutions");
	case 0x0040: return _T("Antex Electronics G.721 ADPCM");
	case 0x0041: return _T("Antex Electronics G.728 CELP");
	case 0x0042: return _T("Microsoft GSM723");
	case 0x0043: return _T("IBM AVC ADPCM");
	case 0x0045: return _T("ITU-T G.726 ADPCM");
	case 0x0050: return _T("MPEG-1 layer 1, 2");
	case 0x0052: return _T("InSoft RT24 (ACM codec is an alternative codec)");
	case 0x0053: return _T("InSoft PAC");
	case 0x0055: return _T("MPEG-1 Layer 3 (MP3)");
	case 0x0057: return _T("AMR-NB");
	case 0x0058: return _T("AMR-WB");
	case 0x0059: return _T("Lucent G.723");
	case 0x0060: return _T("Cirrus Logic");
	case 0x0061: return _T("ESS Technology ESPCM / Duck DK4 ADPCM");
	case 0x0062: return _T("Voxware file-mode codec / Duck DK3 ADPCM");
	case 0x0063: return _T("Canopus Atrac");
	case 0x0064: return _T("APICOM G.726 ADPCM");
	case 0x0065: return _T("APICOM G.722 ADPCM");
	case 0x0066: return _T("Microsoft DSAT");
	case 0x0067: return _T("Microsoft DSAT Display");
	case 0x0069: return _T("Voxware Byte Aligned (bitstream-mode codec)");
	case 0x0070: return _T("Voxware AC8 (Lernout & Hauspie CELP 4.8 kbps)");
	case 0x0071: return _T("Voxware AC10 (Lernout & Hauspie CBS 8kbps)");
	case 0x0072: return _T("Voxware AC16 (Lernout & Hauspie CBS 12kbps)");
	case 0x0073: return _T("Voxware AC20 (Lernout & Hauspie CBS 16kbps)");
	case 0x0074: return _T("Voxware MetaVoice (file and stream oriented)");
	case 0x0075: return _T("Voxware MetaSound (file and stream oriented)");
	case 0x0076: return _T("Voxware RT29HW");
	case 0x0077: return _T("Voxware VR12");
	case 0x0078: return _T("Voxware VR18");
	case 0x0079: return _T("Voxware TQ40");
	case 0x007A: return _T("Voxware SC3");
	case 0x007B: return _T("Voxware SC3");
	case 0x0080: return _T("Softsound");
	case 0x0081: return _T("Voxware TQ60");
	case 0x0082: return _T("Microsoft MSRT24 (ACM codec is an alternative codec)");
	case 0x0083: return _T("AT&T Labs G.729A");
	case 0x0084: return _T("Motion Pixels MVI MV12");
	case 0x0085: return _T("DataFusion Systems G.726");
	case 0x0086: return _T("DataFusion Systems GSM610");
	case 0x0088: return _T("Iterated Systems ISIAudio");
	case 0x0089: return _T("Onlive");
	case 0x008A: return _T("Multitude, Inc. FT SX20");
	case 0x008B: return _T("Infocom ITS A/S G.721 ADPCM");
	case 0x008C: return _T("Convedia G729");
	case 0x008D: return _T("Not specified congruency, Inc.");
	case 0x0091: return _T("Siemens Business Communications SBC24");
	case 0x0092: return _T("Sonic Foundry Dolby AC3 SPDIF");
	case 0x0093: return _T("MediaSonic G.723");
	case 0x0094: return _T("Aculab PLC Prosody 8KBPS");
	case 0x0097: return _T("ZyXEL ADPCM");
	case 0x0098: return _T("Philips LPCBB");
	case 0x0099: return _T("Studer Professional Audio AG Packed");
	case 0x00A0: return _T("Malden Electronics PHONYTALK");
	case 0x00A1: return _T("Racal Recorder GSM");
	case 0x00A2: return _T("Racal Recorder G720.a");
	case 0x00A3: return _T("Racal G723.1");
	case 0x00A4: return _T("Racal Tetra ACELP");
	case 0x00B0: return _T("NEC AAC NEC Corporation");
	case 0x00E1: return _T("Microsoft ADPCM");
	case 0x00FF: return _T("AAC");
	case 0x0100: return _T("Rhetorex ADPCM");
	case 0x0101: return _T("IBM mu-law / BeCubed Software IRAT");
	case 0x0102: return _T("IBM A-law");
	case 0x0103: return _T("IBM AVC ADPCM");
	case 0x0111: return _T("Vivo G.723");
	case 0x0112: return _T("Vivo Siren");
	case 0x0120: return _T("Philips Speech Processing CELP");
	case 0x0121: return _T("Philips Speech Processing GRUNDIG");
	case 0x0123: return _T("Digital G.723");
	case 0x0125: return _T("Sanyo LD ADPCM");
	case 0x0130: return _T("Sipro Lab Telecom ACELP.net");
	case 0x0131: return _T("Sipro Lab Telecom ACELP.4800");
	case 0x0132: return _T("Sipro Lab Telecom ACELP.8V3");
	case 0x0133: return _T("Sipro Lab Telecom ACELP.G.729");
	case 0x0134: return _T("Sipro Lab Telecom ACELP.G.729A");
	case 0x0135: return _T("Sipro Lab Telecom ACELP.KELVIN");
	case 0x0136: return _T("VoiceAge AMR");
	case 0x0140: return _T("Dictaphone G.726 ADPCM");
	case 0x0150: return _T("Qualcomm PureVoice");
	case 0x0151: return _T("Qualcomm HalfRate");
	case 0x0155: return _T("Ring Zero Systems TUB GSM");
	case 0x0160: return _T("Windows Media Audio V1 / DivX audio (WMA)");
	case 0x0161: return _T("Windows Media Audio V2 V7 V8 V9 / DivX audio (WMA) / Alex AC3 Audio");
	case 0x0162: return _T("Windows Media Audio Professional V9");
	case 0x0163: return _T("Windows Media Audio Lossless V9");
	case 0x0164: return _T("WMA Pro over S/PDIF");
	case 0x0170: return _T("UNISYS NAP ADPCM");
	case 0x0171: return _T("UNISYS NAP ULAW");
	case 0x0172: return _T("UNISYS NAP ALAW");
	case 0x0173: return _T("UNISYS NAP 16K");
	case 0x0174: return _T("MM SYCOM ACM SYC008 SyCom Technologies");
	case 0x0175: return _T("MM SYCOM ACM SYC701 G726L SyCom Technologies");
	case 0x0176: return _T("MM SYCOM ACM SYC701 CELP54 SyCom Technologies");
	case 0x0177: return _T("MM SYCOM ACM SYC701 CELP68 SyCom Technologies");
	case 0x0178: return _T("Knowledge Adventure ADPCM");
	case 0x0180: return _T("Fraunhofer IIS MPEG2AAC");
	case 0x0190: return _T("Digital Theater Systems DTS DS");
	case 0x0200: return _T("Creative Labs ADPCM");
	case 0x0202: return _T("Creative Labs FastSpeech8");
	case 0x0203: return _T("Creative Labs FastSpeech10");
	case 0x0210: return _T("UHER Informatic ADPCM");
	case 0x0215: return _T("Ulead DV ACM");
	case 0x0216: return _T("Ulead DV ACM");
	case 0x0220: return _T("Quarterdeck");
	case 0x0230: return _T("I-link Worldwide ILINK VC");
	case 0x0240: return _T("Aureal Semiconductor RAW SPORT");
	case 0x0241: return _T("ESST AC3");
	case 0x0250: return _T("Interactive Products HSX");
	case 0x0251: return _T("Interactive Products RPELP");
	case 0x0260: return _T("Consistent Software CS2");
	case 0x0270: return _T("Sony ATRAC3 (SCX, same as MiniDisk LP2)");
	case 0x0271: return _T("Sony SCY");
	case 0x0272: return _T("Sony ATRAC3");
	case 0x0273: return _T("Sony SPC");
	case 0x0280: return _T("TELUM Telum Inc.");
	case 0x0281: return _T("TELUMIA Telum Inc.");
	case 0x0285: return _T("Norcom Voice Systems ADPCM Norcom Electronics Corporation");
	case 0x0300: return _T("Fujitsu FM TOWNS SND");
	case 0x0301: return _T("Not specified Fujitsu Corporation");
	case 0x0302: return _T("Not specified Fujitsu Corporation");
	case 0x0303: return _T("Not specified Fujitsu Corporation");
	case 0x0304: return _T("Not specified Fujitsu Corporation");
	case 0x0305: return _T("Not specified Fujitsu Corporation");
	case 0x0306: return _T("Not specified Fujitsu Corporation");
	case 0x0307: return _T("Not specified Fujitsu Corporation");
	case 0x0308: return _T("Not specified Fujitsu Corporation");
	case 0x0350: return _T("DEVELOPMENT Micronas Semiconductors, Inc.");
	case 0x0351: return _T("Micronas Semiconductors, Inc. CELP833");
	case 0x0400: return _T("BTV Digital (Brooktree digital audio format)");
	case 0x0401: return _T("Intel Music Coder (IMC)");
	case 0x0402: return _T("Ligos Indeo Audio");
	case 0x0450: return _T("QDesign Music");
	case 0x0500: return _T("On2 VP7 On2 Technologies AVC Audio");
	case 0x0501: return _T("On2 VP6 On2 Technologies");
	case 0x0680: return _T("AT&T Labs VME VMPCM");
	case 0x0681: return _T("AT&T Labs TPC");
	case 0x0700: return _T("YMPEG Alpha (dummy for MPEG-2 compressor)");
	case 0x08AE: return _T("ClearJump LiteWave (lossless)");
	case 0x1000: return _T("Olivetti GSM");
	case 0x1001: return _T("Olivetti ADPCM");
	case 0x1002: return _T("Olivetti CELP");
	case 0x1003: return _T("Olivetti SBC");
	case 0x1004: return _T("Olivetti OPR");
	case 0x1100: return _T("Lernout & Hauspie codec");
	case 0x1101: return _T("Lernout & Hauspie CELP codec");
	case 0x1102: return _T("Lernout & Hauspie SBC codec");
	case 0x1103: return _T("Lernout & Hauspie SBC codec");
	case 0x1104: return _T("Lernout & Hauspie SBC codec");
	case 0x1400: return _T("Norris Communication");
	case 0x1401: return _T("AT&T Labs ISIAudio");
	case 0x1500: return _T("Netspeak GSM - nsgsm32.acm");
	case 0x1501: return _T("Netspeak Truespeech - nstsp32.acm");
	case 0x181C: return _T("VoxWare RT24 speech codec");
	case 0x181E: return _T("Lucent elemedia AX24000P Music codec");
	case 0x1971: return _T("Sonic Foundry LOSSLESS");
	case 0x1979: return _T("Innings Telecom Inc. ADPCM");
	case 0x1C07: return _T("Lucent SX8300P speech codec");
	case 0x1C0C: return _T("Lucent SX5363S G.723 compliant codec");
	case 0x1F03: return _T("CUseeMe DigiTalk (ex-Rocwell)");
	case 0x1FC4: return _T("NCT Soft ALF2CD ACM");
	case 0x2000: return _T("Dolby AC3 / FAST Multimedia AG DVM");
	case 0x2001: return _T("Dolby DTS (Digital Theater System)");
	case 0x2002: return _T("RealAudio 1 / 2 14.4");
	case 0x2003: return _T("RealAudio 1 / 2 28.8");
	case 0x2004: return _T("RealAudio G2 / 8 Cook (low bitrate)");
	case 0x2005: return _T("RealAudio 3 / 4 / 5 Music (DNET)");
	case 0x2006: return _T("RealAudio 10 AAC (RAAC)");
	case 0x2007: return _T("RealAudio 10 AAC+ (RACP)");
	case 0x2500: return _T("Reserved range to 0x2600 Microsoft");
	case 0x3313: return _T("makeAVIS (ffvfw fake AVI sound from AviSynth scripts)");
	case 0x4143: return _T("Divio MPEG-4 AAC audio");
	case 0x4201: return _T("Nokia adaptive multirate Nokia Mobile Phones");
	case 0x4243: return _T("Divio's G726 Divio, Inc.");
	case 0x434C: return _T("LEAD Speech");
	case 0x564C: return _T("LEAD Vorbis");
	case 0x5756: return _T("WavPack Audio");
	case 0x674f: return _T("Ogg Vorbis (mode 1)");
	case 0x6750: return _T("Ogg Vorbis (mode 2)");
	case 0x6751: return _T("Ogg Vorbis (mode 3)");
	case 0x676f: return _T("Ogg Vorbis (mode 1+)");
	case 0x6770: return _T("Ogg Vorbis (mode 2+)");
	case 0x6771: return _T("Ogg Vorbis (mode 3+)");
	case 0x7000: return _T("3COM NBX 3Com Corporation");
	case 0x706D: return _T("FAAD AAC");
	case 0x77A1: return _T("The True Audio");
	case 0x7A21: return _T("GSM-AMR (CBR, no SID)");
	case 0x7A22: return _T("GSM-AMR (VBR, including SID)");
	case 0xA100: return _T("Comverse Infosys Ltd. G723 1");
	case 0xA101: return _T("Comverse Infosys Ltd. AVQSBC");
	case 0xA102: return _T("Comverse Infosys Ltd. OLDSBC");
	case 0xA103: return _T("Symbol Technology's G729A Symbol Technologies Canada");
	case 0xA104: return _T("VoiceAge AMR WB VoiceAge Corporation");
	case 0xA105: return _T("Ingenient Technologies Inc. G726");
	case 0xA106: return _T("ISO/MPEG-4 advanced audio Coding");
	case 0xA107: return _T("Encore Software Ltd's G726");
	case 0xA109: return _T("Speex ACM Codec xiph.org");
	case 0xC0CC: return _T("GigaLink Audio Codec");
	case 0xDFAC: return _T("DebugMode SonicFoundry Vegas FrameServer ACM Codec");
	case 0xe708: return _T("Unknown -");
	case 0xF1AC: return _T("Free Lossless Audio Codec FLAC");
	case 0xFFFC: return _T("VDOwave Audio");
	case 0xFFFE: return _T("Extensible wave format");
	case 0xFFFF: return _T("In Development / Unregistered");
	default:
		return UNKNOWN;
	}
}

/* -------------------------------------------------------------------------- */

CAtlString CWAVFile::GetChannelMode()
{
	CWAVFormatChunk *fmt = mainContainer->getFormatChunk();
	if (fmt == NULL)
		return UNKNOWN;
	if (fmt->ChannelNumber > 2)
		return MULTICHANNEL;

	switch(fmt->ChannelNumber)
	{
	case WAV_CM_MONO:
		return MONO;
	case WAV_CM_STEREO:
		return STEREO;
	default:
		return UNKNOWN;
	}
}

/* -------------------------------------------------------------------------- */

float CWAVFile::GetDuration()
{
	/* Get duration */	
	CWAVFormatChunk *fmt = mainContainer->getFormatChunk();
	if (fmt == NULL)
		return 0.0f;
	long SampleNumber = 0;
	CWAVChunk *chunk = mainContainer->find('data');
	if (chunk == 0)
		return 0.0f;
	long size = (long)chunk->getSize();
	SampleNumber = size / fmt->BlockAlign;
	if (SampleNumber <= 0 && fmt->BytesPerSecond > 0)
		return (float)size / (float) fmt->BytesPerSecond;
	if (SampleNumber > 0 && fmt->SampleRate > 0)
		return (float)SampleNumber / (float) fmt->SampleRate;
	return 0;
}

/* -------------------------------------------------------------------------- */

bool CWAVFile::ReadFromFile(FILE *Stream)
{
	/* Process data if loaded and header valid */
	if (ReadWAV(Stream))
		return true;

	ResetData();
	return false;
}

long CWAVFile::GetBitRate()
{
	CWAVFormatChunk *fmt = mainContainer->getFormatChunk();
	if (fmt == NULL)
		return 0;
	return fmt->BytesPerSecond / 125;
}

bool CWAVFile::IsValid()
{ 
	// Valid ist, wenn  data Tag vorhanden ist
	return (mainContainer->find('data') != NULL);
}

long CWAVFile::GetChannels()
{ 
	CWAVFormatChunk *fmt = mainContainer->getFormatChunk();
	if (fmt == NULL)
		return 0;
	return fmt->ChannelNumber; 
}; 

long CWAVFile::GetSampleRate()
{ 
	CWAVFormatChunk *fmt = mainContainer->getFormatChunk();
	if (fmt == NULL)
		return 0;
	return fmt->SampleRate;
};

long CWAVFile::GetBytesPerSecond()
{ 
	CWAVFormatChunk *fmt = mainContainer->getFormatChunk();
	if (fmt == NULL)
		return 0;
	return fmt->BytesPerSecond;	
};

WORD CWAVFile::GetBlockAlign()
{ 
	CWAVFormatChunk *fmt = mainContainer->getFormatChunk();
	if (fmt == NULL)
		return 0;
	return (WORD)fmt->BlockAlign;
};

WORD CWAVFile::GetBitsPerSample()
{ 
	CWAVFormatChunk *fmt = mainContainer->getFormatChunk();
	if (fmt == NULL)
		return 0;
	if (fmt->BitsPerSample == 0 && fmt->ChannelNumber > 0)
		return (WORD)(fmt->BlockAlign * 8 * fmt->ChannelNumber);
	return (WORD)fmt->BitsPerSample; 
};

long CWAVFile::GetHeaderSize()
{ 
	// FileSize - DataSize = HeaderSize
	CWAVChunk *chunk = mainContainer->find('data');
	if (chunk == 0)
		return CTools::FileSize;
	return CTools::FileSize - (long)chunk->getSize() + 8;
};

short CWAVFile::GetFormatID()
{ 
	CWAVFormatChunk *fmt = mainContainer->getFormatChunk();
	if (fmt == NULL)
		return 0;
	return (short)fmt->FormatID;
};

CAtlString CWAVFile::getTextFrame(u32 FrameID)
{
	CWAVChunk *chunk =  mainContainer->find(FrameID);
	if (chunk == NULL)
		return EMPTY;
	return chunk->getData()->ConvertToUnicodeString(TEXT_ENCODED_ANSI);
}

CAtlString CWAVFile::getDisplayText()
{
	CWAVDISPChunk *chunk = mainContainer->getDispChunk();
	if (chunk == NULL)
		return EMPTY;
	return chunk->getText();
}

void CWAVFile::setDisplayText(LPCWSTR newText)
{
	CWAVDISPChunk *chunk =  mainContainer->addDispChunk();
	chunk->setText(newText);
}

CAtlString CWAVFile::getCartText(BYTE nr)
{
	CWAVCARTChunk *chunk =  mainContainer->getCartChunk();
	if (chunk == NULL)
		return EMPTY;
	return chunk->getASCIIText(nr);
}

void CWAVFile::setCartText(BYTE nr, CAtlString newText)
{
	CWAVCARTChunk *cart =  mainContainer->addCartChunk();
	if (cart != NULL)
		cart->setASCIIText(newText, nr);
}

CAtlString CWAVFile::getBextText(BYTE nr)
{
	CWAVBEXTChunk *chunk =  mainContainer->getBextChunk();
	if (chunk == NULL)
		return EMPTY;
	return chunk->getASCIIText(nr);
}

void CWAVFile::setBextText(BYTE nr, CAtlString newText)
{
	CWAVBEXTChunk *cart =  mainContainer->addBextChunk();
	if (cart != NULL)
		cart->setASCIIText(newText, nr);
}


void CWAVFile::setTextFrame(u32 FrameID, CAtlString newText)
{
	CWAVChunk *chunk =  mainContainer->find(FrameID);
	if (chunk != NULL) //Replace ist einfach
	{	
		chunk->getData()->Clear();
		if (!newText.IsEmpty())
			chunk->getData()->AddEncodedString(TEXT_ENCODED_ANSI, newText, false, true);
		return;
	}
	// Neuanlage
	if (!newText.IsEmpty())
		mainContainer->addInfoChunk(FrameID, newText);
}

CAtlString CWAVFile::getINFOChunkIDs()
{
	CWAVContainer *cont =  mainContainer->getInfoChunk();
	if (cont == NULL)
		return EMPTY;
	CAtlString result;
	CWAVChunk* chunk;
	CSimpleArray<CAtlString> ids;
	size_t count = cont->_children.GetCount();
	u32 frame;
	CAtlString tmp;
	for (size_t i = 0; i < count; i++)
	{
		chunk = cont->_children[i];
		frame = chunk->getID();
		tmp.Format(_T("%c%c%c%c"), BYTE(frame >> 24), BYTE(frame >> 16), BYTE(frame >> 8), BYTE(frame));
		if (ids.Find(tmp) == -1 && chunk->getData()->GetLength() > 0)
		{
			ids.Add(tmp);
			if (result.GetLength() > 0)
				result.Append(_T(","));
			result.Append(tmp);
		}		
	}
	return result;
}

bool CWAVFile::SaveToFile(LPCWSTR FileName)
{
	// Ermittle den Anfangsbereich der Daten
	FILE *Source;
	FILE *Destination;
	CAtlString NewFileName(FileName);
	//long FrameOldSize = 0;
	if ( (Source = _wfsopen(FileName, READ_AND_WRITE, _SH_DENYNO)) == NULL)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	fseek(Source, CTools::ID3v2Size, SEEK_SET);
	CTools::FileSize = _filelength(_fileno(Source));
	CWAVRIFFContainer *newData = new CWAVRIFFContainer();
	if (!newData->load(Source, CTools::ID3v2Size, CTools::FileSize -  CTools::ID3v1Size))
	{
		CTools::instance().setLastError(ERR_INVALID_FORMAT);
		newData->Remove();
		delete newData;
		fclose(Source);
		return false;
	}
	newData->Remove();
	delete newData;

	CTools::instance().writeDebug(_T("Rebuild wav tag"));
	// rebuild File
	NewFileName+=TILDE;
	/* Create file streams */
	if ( (Destination = _wfsopen(NewFileName, WRITE_ONLY, _SH_DENYWR)) == NULL)
	{
		CTools::instance().setLastError(errno);
		fclose(Source);
		return false;
	};
	mainContainer->save(Source, Destination);
	_flushall();
	_fcloseall();
	CTools::instance().doEventsNow();
	/* Replace old file and delete temporary file */
	if (_wremove(FileName) != 0)
	{
		// Lösche auch temp file
		_wremove(NewFileName);
		CTools::instance().setLastError(errno);
		return false;
	}
	CTools::instance().doEventsNow();
	if (_wrename(NewFileName, FileName) != 0)
	{
		CTools::instance().setLastError(errno);
		return false;
	}
	return true;
}
