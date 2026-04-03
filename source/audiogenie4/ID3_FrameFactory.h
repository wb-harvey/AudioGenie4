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

#pragma once
#include "ID3_frame.h"
#include "Blob.h"

#include "ID3F_AENC.h"
#include "ID3F_APIC.h"
#include "ID3F_ASPI.h"
#include "ID3F_CHAP.h"
#include "ID3F_COMM.h"
#include "ID3F_COMR.h"
#include "ID3F_CTOC.h"
#include "ID3F_ENCR.h"
#include "ID3F_EQUA.h"
#include "ID3F_ETCO.h"
#include "ID3F_GEOB.h"
#include "ID3F_GRID.h"
#include "ID3F_LINK.h"
#include "ID3F_MCDI.h"
#include "ID3F_MLLT.h"
#include "ID3F_OWNE.h"
#include "ID3F_PCNT.h"
#include "ID3F_POPM.h"
#include "ID3F_POSS.h"
#include "ID3F_PRIV.h"
#include "ID3F_RBUF.h"
#include "ID3F_RVAD.h"
#include "ID3F_RVRB.h"
#include "ID3F_SEEK.h"
#include "ID3F_SIGN.h"
#include "ID3F_SYLT.h"
#include "ID3F_SYTC.h"
#include "ID3F_T000.h"
#include "ID3F_TXXX.h"
#include "ID3F_UFID.h"
#include "ID3F_USER.h"
#include "ID3F_USLT.h"
#include "ID3F_W000.h"
#include "ID3F_WXXX.h"

#define MAX_EBENEN 5

#define cAENC static_cast<CID3F_AENC*>
#define cAPIC static_cast<CID3F_APIC*>
#define cASPI static_cast<CID3F_ASPI*>
#define cCHAP static_cast<CID3F_CHAP*>
#define cCHAPTER static_cast<CID3F_Chapter*>
#define cCOMM static_cast<CID3F_COMM*>
#define cCOMR static_cast<CID3F_COMR*>
#define cCTOC static_cast<CID3F_CTOC*>
#define cENCR static_cast<CID3F_ENCR*>
#define cEQUA static_cast<CID3F_EQUA*>
#define cETCO static_cast<CID3F_ETCO*>
#define cGEOB static_cast<CID3F_GEOB*>
#define cGRID static_cast<CID3F_GRID*>
#define cLINK static_cast<CID3F_LINK*>
#define cMCDI static_cast<CID3F_MCDI*>
#define cMLLT static_cast<CID3F_MLLT*>
#define cOWNE static_cast<CID3F_OWNE*>
#define cPCNT static_cast<CID3F_PCNT*>
#define cPOPM static_cast<CID3F_POPM*>
#define cPOSS static_cast<CID3F_POSS*>
#define cPRIV static_cast<CID3F_PRIV*>
#define cRBUF static_cast<CID3F_RBUF*>
#define cRVAD static_cast<CID3F_RVAD*>
#define cRVRB static_cast<CID3F_RVRB*>
#define cSEEK static_cast<CID3F_SEEK*>
#define cSIGN static_cast<CID3F_SIGN*>
#define cSYLT static_cast<CID3F_SYLT*>
#define cSYTC static_cast<CID3F_SYTC*>
#define cT000 static_cast<CID3F_T000*>
#define cTXXX static_cast<CID3F_TXXX*>
#define cUFID static_cast<CID3F_UFID*>
#define cUSER static_cast<CID3F_USER*>
#define cUSLT static_cast<CID3F_USLT*>
#define cW000 static_cast<CID3F_W000*>
#define cWXXX static_cast<CID3F_WXXX*>

static enum ID3_FRAMECLASSES {
	ID3_NONE = 0,
	ID3_AENC ,
	ID3_APIC ,
	ID3_ASPI ,
//	ID3_CDM  ,
	ID3_CHAP ,
	ID3_COMM ,
	ID3_COMR ,
	ID3_CTOC ,
//	ID3_CRM  ,
	ID3_ENCR ,
	ID3_EQUA ,
	ID3_EQU2 ,
	ID3_ETCO ,
	ID3_GEOB ,
	ID3_GRID ,
	ID3_LINK ,
	ID3_MCDI ,
	ID3_MLLT ,
	ID3_OWNE ,
	ID3_PCNT ,
	ID3_POPM ,
	ID3_POSS ,
	ID3_PRIV ,
	ID3_RBUF ,
	ID3_RVA2 ,
	ID3_RVAD ,
	ID3_RVRB ,
	ID3_SEEK ,
	ID3_SIGN ,
	ID3_SYLT ,
	ID3_SYTC ,
	ID3_T000 ,
	ID3_TXXX ,
	ID3_UFID ,
	ID3_USER ,
	ID3_USLT ,
	ID3_W000 ,
	ID3_WXXX 
};

static enum ID3_FRAMETYPES {
	F_NONE = 0,
	F_AENC ='AENC',
	F_APIC ='APIC',
	F_ASPI ='ASPI',
	F_BUF  ='BUF',
//	F_CDM  ='CDM',
	F_CHAP ='CHAP',
	F_CNT  ='CNT',
	F_COM  ='COM',
	F_COMM ='COMM',
	F_COMR ='COMR',
	F_CTOC ='CTOC',
	F_CRA  ='CRA',
//	F_CRM  ='CRM',
	F_ENCR ='ENCR',
	F_EQU  ='EQU',
	F_EQU2 ='EQU2',
	F_EQUA ='EQUA',
	F_ETC  ='ETC',
	F_ETCO ='ETCO',
	F_GEO  ='GEO',
	F_GEOB ='GEOB',
	F_GRID ='GRID',
	F_IPL  ='IPL',
	F_IPLS ='IPLS',
	F_LINK ='LINK',
	F_LNK  ='LNK',
	F_MCDI ='MCDI',
	F_MCI  ='MCI',
	F_MLL  ='MLL',
	F_MLLT ='MLLT',
	F_OWNE ='OWNE',
	F_PCNT ='PCNT',
	F_PCST ='PCST', // ITunes Podcast
	F_PIC  ='PIC',
	F_POP  ='POP',
	F_POPM ='POPM',
	F_POSS ='POSS',
	F_PRIV ='PRIV',
	F_RBUF ='RBUF',
	F_REV  ='REV',
	F_RVA  ='RVA',
	F_RVA2 ='RVA2',
	F_RVAD ='RVAD',
	F_RVRB ='RVRB',
	F_SEEK ='SEEK',
	F_SIGN ='SIGN',
	F_SLT  ='SLT',
	F_STC  ='STC',
	F_SYLT ='SYLT',
	F_SYTC ='SYTC',
	F_TAL  ='TAL',
	F_TALB ='TALB',
	F_TBP  ='TBP',
	F_TBPM ='TBPM',
	F_TCAT ='TCAT', // ITUNES PODCAST CATEGORY
	F_TCM  ='TCM',
	F_TCMP ='TCMP', // ITUNES COMPILATION
	F_TCO  ='TCO',
	F_TCOM ='TCOM',
	F_TCON ='TCON',
	F_TCOP ='TCOP',
	F_TCP = 'TCP', // ITUNES
	F_TCR  ='TCR',
	F_TDA  ='TDA',
	F_TDAT ='TDAT',
	F_TDEN ='TDEN',
	F_TDES ='TDES', // ITUNES PODCAST DESC
	F_TDLY ='TDLY', // PLAYLIST DELAY
	F_TDOR ='TDOR', // ORIGINAL YEAR
	F_TDRC ='TDRC', // RECORDING TIME
	F_TDRL ='TDRL', // RELEASE TIME
	F_TDTG ='TDTG', // TAGGING TIME
	F_TDY  ='TDY',
	F_TEN  ='TEN',
	F_TENC ='TENC', // ENCODED BY
	F_TEXT ='TEXT', // LYRICIST / TEXTWRITER
	F_TFLT ='TFLT', // FILE TYPE
	F_TFT  ='TFT',
	F_TGID ='TGID', // ITUNES PODCAST ID
	F_TIM  ='TIM',
	F_TIME ='TIME', // TIME
	F_TIPL ='TIPL', // INVOLVED PEOPLE
	F_TIT1 ='TIT1', // CONTENT GROUP
	F_TIT2 ='TIT2', // TITLE
	F_TIT3 ='TIT3', // SUBTITLE
	F_TKE  ='TKE',
	F_TKEY ='TKEY', // INITIAL KEY
	F_TLA  ='TLA',
	F_TLAN ='TLAN', // LANGUAGE
	F_TLE  ='TLE',
	F_TLEN ='TLEN', // LENGTH
	F_TMCL ='TMCL', // MUSICIAN CREDITS
	F_TMED ='TMED', // MEDIA TYPE
	F_TMOO ='TMOO', // MOOD
	F_TMT  ='TMT',
	F_TOA  ='TOA',
	F_TOAL ='TOAL', // ORIGINAL ALBUM
	F_TOF  ='TOF',
	F_TOFN ='TOFN', // ORIGINAL FILE NAME
	F_TOL  ='TOL',
	F_TOLY ='TOLY', // ORIGLYRICIST / ORIGTEXTWRITER
	F_TOPE ='TOPE', // ORIGINAL ARTIST
	F_TOR  ='TOR',
	F_TORY ='TORY', // ORIGINAL YEAR
	F_TOT  ='TOT',
	F_TOWN ='TOWN', // FILE OWNER
	F_TP1  ='TP1',
	F_TP2  ='TP2',
	F_TP3  ='TP3',
	F_TP4  ='TP4',
	F_TPA  ='TPA',
	F_TPB  ='TPB',
	F_TPE1 ='TPE1', // ARTIST
	F_TPE2 ='TPE2', // BAND /Orchestra
	F_TPE3 ='TPE3', // CONDUCTOR
	F_TPE4 ='TPE4', // INTERPRETED, REMIXED, OR OTHERWISE MODIFIED BY
	F_TPOS ='TPOS', // DISC NUMBER / Part of Set
	F_TPRO ='TPRO', // PRODUCED NOTICE
	F_TPUB ='TPUB', // PUBLISHER
	F_TRC  ='TRC',
	F_TRCK ='TRCK', // TRACK
	F_TRD  ='TRD',
	F_TRDA ='TRDA', // RECORDING DATES
	F_TRK  ='TRK',
	F_TRSN ='TRSN', // NETRADIO STATION
	F_TRSO ='TRSO', // NETRADIO OWNER
	F_TS2  ='TS2',  // ITUNES
	F_TSA  ='TSA',  // ITUNES
	F_TSC  ='TSC',  // ITUNES
	F_TSI  ='TSI',
	F_TSIZ ='TSIZ', // SIZE of FILE without Tags
	F_TSO2 ='TSO2', // ITUNES ID3V2 ALBUMARTIST SORT
	F_TSOA ='TSOA', // ITUNES ALBUM SORT ORDER
	F_TSOC ='TSOC', // ITUNES ID3V2 COMPOSER SORT
	F_TSOP ='TSOP', // ARTIST SORT ORDER
	F_TSOT ='TSOT', // TITLE SORT ORDER
	F_TSP  ='TSP',  // ITUNES
	F_TSRC ='TSRC', // ISRC
	F_TSS  ='TSS',
	F_TSSE ='TSSE', // ENCODER SETTINGS
	F_TSST ='TSST', // SET SUBTITLE
	F_TST  ='TST',   // ITUNES	
	F_TT1  ='TT1',
	F_TT2  ='TT2',
	F_TT3  ='TT3',
	F_TXT  ='TXT',
	F_TXX  ='TXX',
	F_TXXX ='TXXX',
	F_TYE  ='TYE',
	F_TYER ='TYER',
	F_UFI  ='UFI',
	F_UFID ='UFID',
	F_ULT  ='ULT',
	F_USER ='USER',
	F_USLT ='USLT',
	F_WAF  ='WAF',
	F_WAR  ='WAR',
	F_WAS  ='WAS',
	F_WCM  ='WCM',
	F_WCOM ='WCOM',
	F_WCOP ='WCOP',
	F_WCP  ='WCP',
	F_WFED ='WFED', // ITUNES PODCAST URL
	F_WOAF ='WOAF',
	F_WOAR ='WOAR',
	F_WOAS ='WOAS',
	F_WORS ='WORS',
	F_WPAY ='WPAY',
	F_WPB  ='WPB',
	F_WPUB ='WPUB',
	F_WXX  ='WXX',
	F_WXXX ='WXXX',
	F_XDOR ='XDOR', // Release date
	F_XSOP ='XSOP' // Musicbrainz Sortname
};

static const unsigned int ID3_FRAMECODES[][MAX_EBENEN] = {
	/* ID    class     v2.2    v2.3    v2.4 */
	{F_AENC, ID3_AENC, F_CRA , F_AENC, F_AENC },
	{F_APIC, ID3_APIC, F_PIC , F_APIC, F_APIC },
	{F_ASPI, ID3_ASPI, F_NONE, F_NONE, F_ASPI },
//	{F_CDM , ID3_CDM , F_CDM , F_NONE, F_NONE },
	{F_CHAP, ID3_CHAP, F_NONE, F_CHAP, F_CHAP },
	{F_COMM, ID3_COMM, F_COM , F_COMM, F_COMM },
	{F_COMR, ID3_COMR, F_NONE, F_COMR, F_COMR },
	{F_CTOC, ID3_CTOC, F_NONE, F_CTOC, F_CTOC },
//	{F_CRM , ID3_CRM , F_CRM , F_NONE, F_NONE },
	{F_ENCR, ID3_ENCR, F_NONE, F_ENCR, F_ENCR },
	{F_EQUA, ID3_EQUA, F_EQU , F_EQUA, F_EQU2 },
	{F_EQU2, ID3_EQUA, F_EQU , F_EQUA, F_EQU2 },
	{F_ETCO, ID3_ETCO, F_ETC , F_ETCO, F_ETCO },
	{F_GEOB, ID3_GEOB, F_GEO , F_GEOB, F_GEOB },
	{F_GRID, ID3_GRID, F_NONE, F_GRID, F_GRID },
	{F_LINK, ID3_LINK, F_LNK , F_LINK, F_LINK },
	{F_MCDI, ID3_MCDI, F_MCI , F_MCDI, F_MCDI },
	{F_MLLT, ID3_MLLT, F_MLL , F_MLLT, F_MLLT },
	{F_OWNE, ID3_OWNE, F_NONE, F_OWNE, F_OWNE },
	{F_PCNT, ID3_PCNT, F_CNT , F_PCNT, F_PCNT },
// 	{F_PCST, ID3_T000, F_NONE, F_PCST, F_PCST }, // EIGENTLICH KEIN TEXTFRAME, SEPARAT HANDELN TODO
	{F_POPM, ID3_POPM, F_POP , F_POPM, F_POPM },
	{F_POSS, ID3_POSS, F_NONE, F_POSS, F_POSS },
	{F_PRIV, ID3_PRIV, F_NONE, F_PRIV, F_PRIV },
	{F_RBUF, ID3_RBUF, F_BUF , F_RBUF, F_RBUF },
	{F_RVAD, ID3_RVAD, F_RVA , F_RVAD, F_RVA2 },
	{F_RVA2, ID3_RVAD, F_RVA , F_RVAD, F_RVA2 },
	{F_RVRB, ID3_RVRB, F_REV , F_RVRB, F_RVRB },
	{F_SEEK, ID3_SEEK, F_NONE, F_NONE, F_SEEK },
	{F_SIGN, ID3_SIGN, F_NONE, F_NONE, F_SIGN },
	{F_SYLT, ID3_SYLT, F_SLT , F_SYLT, F_SYLT },
	{F_SYTC, ID3_SYTC, F_STC , F_SYTC, F_SYTC },
	{F_TALB, ID3_T000, F_TAL , F_TALB, F_TALB },
	{F_TBPM, ID3_T000, F_TBP , F_TBPM, F_TBPM },
	{F_TCAT, ID3_T000, F_NONE, F_TCAT, F_TCAT },
	{F_TCMP, ID3_T000, F_TCP , F_TCMP, F_TCMP },
	{F_TCOM, ID3_T000, F_TCM , F_TCOM, F_TCOM },
	{F_TCON, ID3_T000, F_TCO , F_TCON, F_TCON },
	{F_TCOP, ID3_T000, F_TCR , F_TCOP, F_TCOP },
	{F_TDAT, ID3_T000, F_TDA , F_TDAT, F_NONE },
	{F_TDEN, ID3_T000, F_NONE, F_NONE, F_TDEN },
	{F_TDES, ID3_T000, F_NONE, F_TDES, F_TDES },
	{F_TDLY, ID3_T000, F_TDY , F_TDLY, F_TDLY },
	{F_TDOR, ID3_T000, F_NONE, F_NONE, F_TDOR },
	{F_TDRC, ID3_T000, F_NONE, F_NONE, F_TDRC },
	{F_TDRL, ID3_T000, F_NONE, F_NONE, F_TDRL },
	{F_TDTG, ID3_T000, F_NONE, F_NONE, F_TDTG },
	{F_TENC, ID3_T000, F_TEN , F_TENC, F_TENC },
	{F_TEXT, ID3_T000, F_TXT , F_TEXT, F_TEXT },
	{F_TFLT, ID3_T000, F_TFT , F_TFLT, F_TFLT },
	{F_TGID, ID3_T000, F_NONE, F_TGID, F_TGID },
	{F_TIME, ID3_T000, F_TIM , F_TIME, F_NONE },
	{F_TIPL, ID3_T000, F_IPL , F_IPLS, F_TIPL },
	{F_TIT1, ID3_T000, F_TT1 , F_TIT1, F_TIT1 },
	{F_TIT2, ID3_T000, F_TT2 , F_TIT2, F_TIT2 },
	{F_TIT3, ID3_T000, F_TT3 , F_TIT3, F_TIT3 },
	{F_TKEY, ID3_T000, F_TKE , F_TKEY, F_TKEY },
	{F_TLAN, ID3_T000, F_TLA , F_TLAN, F_TLAN },
	{F_TLEN, ID3_T000, F_TLE , F_TLEN, F_TLEN },
	{F_TMCL, ID3_T000, F_NONE, F_NONE, F_TMCL },
	{F_TMED, ID3_T000, F_TMT , F_TMED, F_TMED },
	{F_TMOO, ID3_T000, F_NONE, F_NONE, F_TMOO },
	{F_TOAL, ID3_T000, F_TOT , F_TOAL, F_TOAL },
	{F_TOFN, ID3_T000, F_TOF , F_TOFN, F_TOFN },
	{F_TOLY, ID3_T000, F_TOL , F_TOLY, F_TOLY },
	{F_TOPE, ID3_T000, F_TOA , F_TOPE, F_TOPE },
	{F_TORY, ID3_T000, F_TOR , F_TORY, F_NONE },
	{F_TOWN, ID3_T000, F_NONE, F_TOWN, F_TOWN },
	{F_TPE1, ID3_T000, F_TP1 , F_TPE1, F_TPE1 },
	{F_TPE2, ID3_T000, F_TP2 , F_TPE2, F_TPE2 },
	{F_TPE3, ID3_T000, F_TP3 , F_TPE3, F_TPE3 },
	{F_TPE4, ID3_T000, F_TP4 , F_TPE4, F_TPE4 },
	{F_TPOS, ID3_T000, F_TPA , F_TPOS, F_TPOS },
	{F_TPRO, ID3_T000, F_NONE, F_NONE, F_TPRO },
	{F_TPUB, ID3_T000, F_TPB , F_TPUB, F_TPUB },
	{F_TRCK, ID3_T000, F_TRK , F_TRCK, F_TRCK },
	{F_TRDA, ID3_T000, F_TRD , F_TRDA, F_NONE },
	{F_TRSN, ID3_T000, F_NONE, F_TRSN, F_TRSN },
	{F_TRSO, ID3_T000, F_NONE, F_TRSO, F_TRSO },
	{F_TSIZ, ID3_T000, F_TSI , F_TSIZ, F_NONE },
	{F_TSO2, ID3_T000, F_TS2 , F_TSO2, F_TSO2 },
	{F_TSOA, ID3_T000, F_TSA , F_TSOA, F_TSOA },
	{F_TSOP, ID3_T000, F_TSP , F_TSOP, F_TSOP },
	{F_TSOC, ID3_T000, F_TSC , F_TSOC, F_TSOC },
	{F_TSOT, ID3_T000, F_TST , F_TSOT, F_TSOT },
	{F_TSRC, ID3_T000, F_TRC , F_TSRC, F_TSRC },
	{F_TSSE, ID3_T000, F_TSS , F_TSSE, F_TSSE },
	{F_TSST, ID3_T000, F_NONE, F_NONE, F_TSST },
	{F_TXXX, ID3_TXXX, F_TXX , F_TXXX, F_TXXX },
	{F_TYER, ID3_T000, F_TYE , F_TYER, F_NONE },
	{F_UFID, ID3_UFID, F_UFI , F_UFID, F_UFID },
	{F_USER, ID3_USER, F_NONE, F_USER, F_USER },
	{F_USLT, ID3_USLT, F_ULT , F_USLT, F_USLT },
	{F_WCOM, ID3_W000, F_WCM , F_WCOM, F_WCOM },
	{F_WCOP, ID3_W000, F_WCP , F_WCOP, F_WCOP },
	{F_WFED, ID3_W000, F_NONE, F_WFED, F_WFED },
	{F_WOAF, ID3_W000, F_WAF , F_WOAF, F_WOAF },
	{F_WOAR, ID3_W000, F_WAR , F_WOAR, F_WOAR },
	{F_WOAS, ID3_W000, F_WAS , F_WOAS, F_WOAS },
	{F_WORS, ID3_W000, F_NONE, F_WORS, F_WORS },
	{F_WPAY, ID3_W000, F_NONE, F_WPAY, F_WPAY },
	{F_WPUB, ID3_W000, F_WPB , F_WPUB, F_WPUB },
	{F_WXXX, ID3_WXXX, F_WXX , F_WXXX, F_WXXX },
	{F_XDOR, ID3_T000, F_NONE, F_XDOR, F_XDOR },
	{F_XSOP, ID3_T000, F_NONE, F_XSOP, F_XSOP }
};

static int anzKnownFrames;

static CAtlMap<u32, u32> frameClassMap;
static CAtlMap<u32, u32> frameIDsMap;
static CAtlMap<u32, u32>::CPair *pPair;

class CID3_FrameFactory
{
public:
	static CID3_FrameFactory &instance();
	static CID3_Frame *createFrame(const u32 oldID);
	static u32 findUniqueFrameID(u32 oldID) ;
	static u32 findFrameClassType(u32 oldID);
	static u32 findTagForVersion(u32 uniqueID);	
protected:
	CID3_FrameFactory();
	virtual ~CID3_FrameFactory();
private:
	CID3_FrameFactory(const CID3_FrameFactory &);
	CID3_FrameFactory &operator=(const CID3_FrameFactory &);	
};

