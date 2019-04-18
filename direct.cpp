/*

  SjASM Z80 Assembler

  Copyright (c) 2006 Sjoerd Mastijn

  This software is provided 'as-is', without any express or implied warranty.
  In no event will the authors be held liable for any damages arising from the
  use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it freely,
  subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim
     that you wrote the original software. If you use this software in a product,
     an acknowledgment in the product documentation would be appreciated but is
     not required.

  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

  3. This notice may not be removed or altered from any source distribution.

*/

// direct.cpp

#include "sjasm.h"

funtabcls dirtab;

int ParseDirective() {
  char *olp=lp;
  char *n;
  bp=lp;
  if (!(n=getinstr(lp))) {
    if (*lp=='#' && *(lp+1)=='#') {
      lp+=2;
      aint val;
      synerr=0; if (!ParseExpression(lp,val)) val=4; synerr=1;
      mapadr+=((~mapadr+1)&(val-1));
      return 1;
    }
    else { lp=olp;  return 0; }
  }
  if (dirtab.zoek(n)) { return 1; }
  lp=olp;
  return 0;
}

#ifdef SECTIONS
void dirPOOL() {
  sections osection=section;
  section=POOL;
  pooltab.emit();
  section=osection;
}

void dirTEXT() {
  section=TEXT;
}

void dirDATA() {
  section=DATA;
}
#endif

void dirBYTE() {
  int teller,e[129];
  teller=getBytes(lp,e,0,0);
  if (!teller) { error(".byte with no arguments",0); return; }
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitBytes(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitBytes(e);
#endif
}

void dirDC() {
  int teller,e[129];
  teller=getBytes(lp,e,0,1);
  if (!teller) { error(".byte with no arguments",0); return; }
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitBytes(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitBytes(e);
#endif
}

void dirDZ() {
  int teller,e[130];
  teller=getBytes(lp,e,0,0);
  if (!teller) { error(".byte with no arguments",0); return; }
  e[teller++]=0; e[teller]=-1;
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitBytes(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitBytes(e);
#endif
}

void dirABYTE() {
  aint add;
  int teller=0,e[129];
  if (ParseExpression(lp,add)) {
    check8(add); add&=255;
    teller=getBytes(lp,e,add,0);
    if (!teller) { error(".abyte with no arguments",0); return; }
#ifdef SECTIONS
    switch (section) {
    case TEXT: case POOL: EmitBytes(e); break;
    case DATA: pooltab.add(bp); break;
    default: error ("Unknown section",0,FATAL); break;
    }
#else
    EmitBytes(e);
#endif
  } else error("Expression expected",0);
}

void dirABYTEC() {
  aint add;
  int teller=0,e[129];
  if (ParseExpression(lp,add)) {
    check8(add); add&=255;
    teller=getBytes(lp,e,add,1);
    if (!teller) { error(".abyte with no arguments",0); return; }
#ifdef SECTIONS
    switch (section) {
    case TEXT: case POOL: EmitBytes(e); break;
    case DATA: pooltab.add(bp); break;
    default: error ("Unknown section",0,FATAL); break;
    }
#else
    EmitBytes(e);
#endif
  } else error("Expression expected",0);
}

void dirABYTEZ() {
  aint add;
  int teller=0,e[129];
  if (ParseExpression(lp,add)) {
    check8(add); add&=255;
    teller=getBytes(lp,e,add,0);
    if (!teller) { error(".abyte with no arguments",0); return; }
    e[teller++]=0; e[teller]=-1;
#ifdef SECTIONS
    switch (section) {
    case TEXT: case POOL: EmitBytes(e); break;
    case DATA: pooltab.add(bp); break;
    default: error ("Unknown section",0,FATAL); break;
    }
#else
    EmitBytes(e);
#endif
  } else error("Expression expected",0);
}

void dirBYTEL() {
  int teller,e[130];
  teller=getBytes(lp,e+1,0,0);
  e[0]=teller;
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitBytes(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitBytes(e);
#endif
}

void dirBYTER() {
  int teller,e[129];
  teller=getBytes(lp,e,0,0);
  if (!teller) { error(".dbr with no arguments",0); return; }
  for (int i=0; i<teller; i++) e[i] &= 0x7f;
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitBytes(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitBytes(e);
#endif
}

void dirBYTES() {
  int teller,e[129];
  teller=getBytes(lp,e,0,0);
  if (!teller) { error(".dbs with no arguments",0); return; }
  for (int i=0; i<teller; i++) e[i] |= 0x80;
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitBytes(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitBytes(e);
#endif
}

void dirPIXEL2() {
  int teller,e[129];
  teller=getBytes(lp,e,0,0);
  if (!teller) { error(".dp2 with no arguments",0); return; }
  while (teller&7) e[teller++]=0;
  for (int i=0; i<teller; i++) e[i>>3] = ((e[i>>3]&0x7f)<<1) | (e[i]&0x01);
  teller=teller>>3; e[teller]=-1;
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitBytes(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitBytes(e);
#endif
}

void dirPIXEL4() {
  int teller,e[129];
  teller=getBytes(lp,e,0,0);
  if (!teller) { error(".dp4 with no arguments",0); return; }
  while (teller&3) e[teller++]=0;
  for (int i=0; i<teller; i++) e[i>>2] = ((e[i>>2]&0x3f)<<2) | (e[i]&0x03);
  teller=teller>>2; e[teller]=-1;
  for (int i=0; i<teller; i++) {
    e[i] =   ((e[i]&0x80)>>4) | ((e[i]&0x40)<<1)
           | ((e[i]&0x20)>>3) | ((e[i]&0x10)<<2)
           | ((e[i]&0x08)>>2) | ((e[i]&0x04)<<3)
           | ((e[i]&0x02)>>1) | ((e[i]&0x01)<<4);
  }
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitBytes(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitBytes(e);
#endif
}

void dirPIXEL16() {
  int teller,e[129];
  teller=getBytes(lp,e,0,0);
  if (!teller) { error(".dp16 with no arguments",0); return; }
  if (teller&1) e[teller++]=0;
  for (int i=0; i<teller; i++) e[i>>1] = ((e[i>>1]&0x0f)<<4) | (e[i]&0x0f);
  teller=teller>>1; e[teller]=-1;
  for (int i=0; i<teller; i++) {
    e[i] =   ((e[i]&0x80)>>6) | ((e[i]&0x40)>>1)
           | ((e[i]&0x20)>>2) | ((e[i]&0x10)<<3)
           | ((e[i]&0x08)>>3) | ((e[i]&0x04)<<2)
           | ((e[i]&0x02)<<1) | ((e[i]&0x01)<<6);
  }
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitBytes(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitBytes(e);
#endif
}

void dirWORD() {
  aint val;
  int teller=0,e[129];
  skipblanks();
  while (*lp) {
    if (ParseExpression(lp,val)) {
      check16(val);
      if (teller>127) error("Over 128 values in .word",0,FATAL);
      e[teller++]=val & 65535;
    } else { error("Syntax error",lp,CATCHALL); return; }
    skipblanks();
    if (*lp!=',') break;
    ++lp; skipblanks();
  }
  e[teller]=-1;
  if (!teller) { error(".word with no arguments",0); return; }
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitWords(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitWords(e);
#endif
}

void dirDWORD() {
  aint val;
  int teller=0,e[129*2];
  skipblanks();
  while (*lp) {
    if (ParseExpression(lp,val)) {
      if (teller>127) error("Over 128 values in .dword",0,FATAL);
      e[teller*2]=val & 65535; e[teller*2+1]=(unsigned int)val >> 16; ++teller;
    } else { error("Syntax error",lp,CATCHALL); return; }
    skipblanks();
    if (*lp!=',') break;
    ++lp; skipblanks();
  }
  e[teller*2]=-1;
  if (!teller) { error(".dword with no arguments",0); return; }
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitWords(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitWords(e);
#endif
}

void dirD24() {
  aint val;
  int teller=0,e[129*3];
  skipblanks();
  while (*lp) {
    if (ParseExpression(lp,val)) {
      check24(val);
      if (teller>127) error("Over 128 values in .d24",0,FATAL);
      e[teller*3]=val & 255; e[teller*3+1]=(val>>8)&255; e[teller*3+2]=(val>>16)&255; ++teller;
    } else { error("Syntax error",lp,CATCHALL); return; }
    skipblanks();
    if (*lp!=',') break;
    ++lp; skipblanks();
  }
  e[teller*3]=-1;
  if (!teller) { error(".d24 with no arguments",0); return; }
#ifdef SECTIONS
  switch (section) {
  case TEXT: case POOL: EmitBytes(e); break;
  case DATA: pooltab.add(bp); break;
  default: error ("Unknown section",0,FATAL); break;
  }
#else
  EmitBytes(e);
#endif
}

void dirBLOCK() {
  aint teller,val=0;
  if (ParseExpression(lp,teller)) {
    if ((signed)teller<0) { error("block with negative size",0); teller=0; }
    if (comma(lp)) ParseExpression(lp,val);
#ifdef SECTIONS
    switch (section) {
    case TEXT: case POOL: EmitBlock(val,teller); break;
    case DATA: pooltab.add(bp); break;
    default: error ("Unknown section",0,FATAL); break;
    }
#else
  EmitBlock(val,teller);
#endif
  } else error("Syntax Error",lp,CATCHALL);
}

void dirORG() {
  aint val;
#ifdef SECTIONS
  if (section!=TEXT) { error(".org only allowed in text sections",0); *lp=0; return; }
#endif
  if (relocpass) error(".org not allowed in relocatable modules",0,FATAL);
  if (ParseExpression(lp,val)) adres=val; else error("Syntax error",0,CATCHALL);
}

void dirHEADER() {
  int e[9];
  aint val;
#ifdef SECTIONS
  if (section!=TEXT) { error("HEADER only allowed in text sections",0); *lp=0; return; }
#endif
  if (!ParseExpression(lp,val)) error("Syntax error",0,CATCHALL);
  if ((val<4 || val>7) && val!=2) error("Invalid or unsupported HEADER type",0,FATAL);
  if (adres && !relocpass) error("HEADER not at the beginning of module",0,FATAL);
  exoshdrtype = val;
  if (val<5 || val>6) return;
  adres = (val!=6 ? 0x00f0 : 0xbffa);
  e[0] = int(val<<8);
  if (!comma(lp)) val=(size==-1 ? aint(endadres):(size+adres));
  else ParseExpression(lp,val);
  e[1] = int(val-(adres+16)); check16(e[1]);
  if (e[1]>(adres<0x8000 ? 0xbf00 : 0x3ff6)) error("Bytes lost",0);
  for (int i=2; i<8; i++) e[i] = 0;
  e[8] = -1;
  EmitWords(e);
}

void dirMAP() {
#ifdef SECTIONS
  if (section!=TEXT) { error(".map only allowed in text sections",0); *lp=0; return; }
#endif
  maplstp=new adrlst(mapadr,maplstp);
  aint val;
  labelnotfound=0;
  if (ParseExpression(lp,val)) mapadr=val; else error("Syntax error",0,CATCHALL);
  if (labelnotfound) error("Forward reference",0,ALL);
}

void dirENDMAP() {
#ifdef SECTIONS
  if (section!=TEXT) { error(".endmap only allowed in text sections",0); *lp=0; return; }
#endif
  if (maplstp) { mapadr=maplstp->val; maplstp=maplstp->next; }
  else error(".endmodule without module",0);
}


void dirALIGN() {
  aint val;
  if (!ParseExpression(lp,val)) val=4;
  if (val<1 || val>32768 || (val&(val-1))!=0) error("Illegal align",0);
  else {
    val=(~adres+1)&(val-1);
    EmitBlock(0,val);
  }
}

void dirMODULE() {
#ifdef SECTIONS
  if (section!=TEXT) { error(".module only allowed in text sections",0); *lp=0; return; }
#endif
  modlstp=new stringlst(modlabp,modlstp);
  char *n;
  skipblanks(lp); if (!*lp) { modlabp=0; return; }
  if ((n=getid(lp)) != (char *) 0) modlabp=n;
  else error("Syntax error",0,CATCHALL);
}

void dirENDMODULE() {
#ifdef SECTIONS
  if (section!=TEXT) { error(".endmodule only allowed in text sections",0); *lp=0; return; }
#endif
  if (modlstp) { modlabp=modlstp->string; modlstp=modlstp->next; }
  else error(".endmodule without module",0);
}

void dirZ80() {
#ifdef SECTIONS
  dirPOOL();
  section=TEXT;
#endif
#ifdef METARM
  cpu=Z80;
#endif
  piCPUp=piZ80;
}

void dirARM() {
#ifdef METARM
#ifdef SECTIONS
  dirPOOL();
  section=TEXT;
#endif
  cpu=ARM; piCPUp=piARM;
#else
  error("No ARM support in this version",0,FATAL);
#endif
}

void dirTHUMB() {
#ifdef METARM
#ifdef SECTIONS
  dirPOOL();
  section=TEXT;
#endif
  cpu=THUMB; piCPUp=piTHUMB;
#else
  error("No ARM support in this version",0,FATAL);
#endif
}

void dirEND() {
#ifdef SECTIONS
  dirPOOL();
#endif
  running=0;
}

void dirSIZE() {
  aint val;
#ifdef SECTIONS
  if (section!=TEXT) error(".size is only allowed in text sections",0,FATAL);
#endif
  if (!ParseExpression(lp,val)) { error("Syntax error",bp,CATCHALL); return; }
  if (pass==2) return;
  if (relocpass) val=val<<1;
  else if (size!=(aint)-1) { error("Multiple sizes?",0,PASS1); return; }
  size=val;
}

void dirINCBIN() {
  aint val;
  char *fnaam;
  int offset=-1,length=-1;
#ifdef SECTIONS
  if (section!=TEXT) error(".include only allowed in text sections",0,FATAL);
#endif
  fnaam=getfilename(lp);
  if (comma(lp)) {
    if (!comma(lp)) {
      if (!ParseExpression(lp,val)) { error("Syntax error",bp,CATCHALL); return; }
      if (val<0) { error("Negative values are not allowed",bp); return; }
      offset=val;
    }
    if (comma(lp)) {
      if (!ParseExpression(lp,val)) { error("Syntax error",bp,CATCHALL); return; }
      if (val<0) { error("Negative values are not allowed",bp); return; }
      length=val;
    }
  }
  BinIncFile(fnaam,offset,length);
}

void dirFSIZE() {
  char *fnaam;
  long fsize=-1;
  int e[5];
#ifdef SECTIONS
  if (section!=TEXT) error(".fsize only allowed in text sections",0,FATAL);
#endif
  fnaam=getfilename(lp);
  if (fnaam && *fnaam) {
    FILE *f=fopen(fnaam,"rb");
    if (f) {
      if (fseek(f,0,SEEK_END)>=0) fsize=ftell(f);
      fclose(f);
    }
  }
  e[0]=2;
  if (comma(lp)) {
    e[0]=0;
    char *lnaam=getid(lp);
    if (lnaam) {
      char *lnaam2=lnaam;
      if ((e[0]=needa(lnaam2,"bc",0x01,"de",0x11,"hl",0x21))==0 &&
          (e[0]=needa(lnaam2,"sp",0x31,"ix",0xdd,"iy",0xfd))==0) {
        if ((lnaam2=MaakLabNaam(lnaam))!=(char *)0) {
          if (!labtab.insert(lnaam2,aint(fsize)) && !relocpass)
            error("Duplicate label",lnaam2,PASS1);
          delete[] lnaam2;
        }
      }
      free(lnaam);
    }
  }
  if (e[0]!=0) {
    if (fsize<0) error("Error opening file",fnaam,FATAL);
    int i=(e[0]&1)+int(bool(e[0]&0x80));
    e[1]=0x21; e[i]=fsize&0xff; e[i+1]=(fsize>>8)&0xff; e[i+2]=-1;
    EmitBytes(e);
    if (fsize>0xffff) error("Bytes lost",0);
  }
}

void dirTEXTAREA() {
#ifdef SECTIONS
  if (section!=TEXT) { error(".textarea only allowed in text sections",0); *lp=0; return; }
#endif
  aint oadres=adres,val;
  labelnotfound=0;
  if (!ParseExpression(lp,val)) { error("No adress given",0); return; }
  if (labelnotfound) error("Forward reference",0,ALL);
  ListFile();
  adres=val; if (ReadFile()!=ENDTEXTAREA) error("No end of textarea",0);
#ifdef SECTIONS
  dirPOOL();
#endif
  adres=oadres+adres-val;
}

void dirIF() {
  aint val;
  labelnotfound=0;
  if (!ParseExpression(lp,val)) { error("Syntax error",0,CATCHALL); return; }
  if (labelnotfound) error("Forward reference",0,ALL);
  if (val) {
    ListFile();
    switch (ReadFile()) {
    case ELSE: if (SkipFile()!=ENDIF) error("No endif",0); break;
    case ENDIF: break;
    default: error("No endif!",0); break;
    }
  }
  else {
    ListFile();
    switch (SkipFile()) {
    case ELSE: if (ReadFile()!=ENDIF) error("No endif",0); break;
    case ENDIF: break;
    default: error("No endif!",0); break;
    }
  }
  *lp=0;
}

void dirELSE() {
  error("Else without if",0);
}

void dirENDIF() {
  error("Endif without if",0);
}

void dirENDTEXTAREA() {
  error("Endt without textarea",0);
}

void dirINCLUDE() {
  char *fnaam;
#ifdef SECTIONS
  if (section!=TEXT) error(".include only allowed in text sections",0,FATAL);
#endif
  fnaam=getfilename(lp);
  ListFile(); OpenFile(fnaam); donotlist=1;
}

void dirOUTPUT() {
  char *fnaam;
#ifdef SECTIONS
  if (section!=TEXT) error(".output only allowed in text sections",0,FATAL);
#endif
  if (relocpass) error(".output not allowed in relocatable modules",0,FATAL);
  fnaam=getfilename(lp); if (fnaam[0]=='<') fnaam++;
  int mode=OUTPUT_TRUNCATE;
  if(comma(lp))
  {
    char modechar=(*lp) | 0x20;
    lp++;
    if(modechar=='t') mode=OUTPUT_TRUNCATE;
    else if(modechar=='r') mode=OUTPUT_REWIND;
    else if(modechar=='a') mode=OUTPUT_APPEND;
    else error("Syntax error",bp,CATCHALL);
  }
  if (pass==2) NewDest(fnaam,mode);
}

void dirDEFINE() {
  char *id;
  char *p=line;
#ifdef SECTIONS
  if (section!=TEXT) { error(".define only allowed in text sections",0); *lp=0; return; }
#endif
  while ('o') {
    if (!*p) error("define error",0,FATAL);
    if (*p=='.') { ++p; continue; }
    if (*p=='d' || *p=='D') break;
    ++p;
  }
  if (!cmphstr(p,"define")) error("define error",0,FATAL);
  if (!(id=getid(p))) { error("illegal define",0); return; }
  if (pass==1 && !relocpass) definetab.add(id,p);
  while (*lp) ++lp;
}

void dirIFDEF() {
  char *p=line,*id;
  while ('o') {
    if (!*p) error("ifdef error",0,FATAL);
    if (*p=='.') { ++p; continue; }
    if (*p=='i' || *p=='I') break;
    ++p;
  }
  if (!cmphstr(p,"ifdef")) error("ifdef error",0,FATAL);
  Ending res;
  if (!(id=getid(p))) { error("Illegal identifier",0,PASS1); return; }
  if (definetab.bestaat(id)) {
    ListFile();
    switch (res=ReadFile()) {
    case ELSE: if (SkipFile()!=ENDIF) error("No endif",0); break;
    case ENDIF: break;
    default: error("No endif!",0); break;
    }
  }
  else {
    ListFile();
    switch (res=SkipFile()) {
    case ELSE: if (ReadFile()!=ENDIF) error("No endif",0); break;
    case ENDIF: break;
    default: error("No endif!",0); break;
    }
  }
  *lp=0;
}

void dirIFNDEF() {
  char *p=line,*id;
  while ('o') {
    if (!*p) error("ifndef error",0,FATAL);
    if (*p=='.') { ++p; continue; }
    if (*p=='i' || *p=='I') break;
    ++p;
  }
  if (!cmphstr(p,"ifndef")) error("ifndef error",0,FATAL);
  Ending res;
  if (!(id=getid(p))) { error("Illegal identifier",0,PASS1); return; }
  if (!definetab.bestaat(id)) {
    ListFile();
    switch (res=ReadFile()) {
    case ELSE: if (SkipFile()!=ENDIF) error("No endif",0); break;
    case ENDIF: break;
    default: error("No endif!",0); break;
    }
  }
  else {
    ListFile();
    switch (res=SkipFile()) {
    case ELSE: if (ReadFile()!=ENDIF) error("No endif",0); break;
    case ENDIF: break;
    default: error("No endif!",0); break;
    }
  }
  *lp=0;
}

void dirEXPORT() {
  aint val;
  char *n,*p;
  if (pass==1) return;
  if (!(n=p=getid(lp))) { error("Syntax error",lp,CATCHALL); return; }
  labelnotfound=0;
  getLabelValue(n,val); if (labelnotfound) { error("Label not found",p,SUPPRES); return; }
  WriteExp(p,val);
}

void dirMACRO() {
#ifdef SECTIONS
  if (section!=TEXT) error("macro definitions only allowed in text sections",0,FATAL);
#endif

  if (lijst) error("No macro definitions allowed here",0,FATAL);
  char *n;
  if (!(n=getid(lp))) { error("Illegal macroname",0,PASS1); return; }
  macrotab.add(n,lp);
}

void dirENDM() {
  error("End macro without macro",0);
}

void dirENDS() {
  error("End structre without structure",0);
}

void dirASSERT() {
  char *p=lp;
  aint val;
  if (!ParseExpression(lp,val)) { error("Syntax error",0,CATCHALL); return; }
  if (pass==2 && !val) error("Assertion failed",p);
  *lp=0;
}

void dirSTRUCT() {
#ifdef SECTIONS
  if (section!=TEXT) error("structure definitions only allowed in text sections",0,FATAL);
#endif
  structcls *st;
  int global=0;
  aint offset=0,bind=0;
  char *naam;
  skipblanks();
  if (*lp=='@') { ++lp; global=1; }
  if (!(naam=getid(lp))) { error("Illegal structurename",0,PASS1); return; }
  if (comma(lp)) {
    labelnotfound=0;
    if (!ParseExpression(lp,offset)) { error("Syntax error",0,CATCHALL); return; }
    if (labelnotfound) error("Forward reference",0,ALL);
  }
  st=structtab.add(naam,offset,bind,global);
  ListFile();
  while ('o') {
    if (!ReadLine()) { error("Unexpected end of structure",0,PASS1); break; }
    lp=line; if (white()) { skipblanks(lp); if (*lp=='.') ++lp; if (cmphstr(lp,"ends")) break; }
    ParseStructLine(st);
    ListFileSkip(line);
  }
  st->deflab();
}

void dirFORG() {
  aint val;
  int method=SEEK_SET;
  if (relocpass) error(".fpos not allowed in relocatable modules",0,FATAL);
  skipblanks(lp);
  if((*lp=='+') || (*lp=='-')) method=SEEK_CUR;
  if (!ParseExpression(lp,val)) error("Syntax error",0,CATCHALL);
  if (pass==2) SeekDest(val,method);
}

/*
void dirBIND() {
}
*/

void dirREPT() {
  aint val;
  char *ml;
  int olistmacro;
  stringlst *s,*f;
  labelnotfound=0;
  if (!ParseExpression(lp,val)) { error("Syntax error",0,CATCHALL); return; }
  if (labelnotfound) error("Forward reference",0,ALL);
  if ((int)val<0) { error("Illegal repeat value",0,CATCHALL); return; }
  ListFile();
  if (!ReadFileToStringLst(f,"endm")) error("Unexpected end of repeat",0,PASS1);
  ListFile();
  olistmacro=listmacro; listmacro=1; ml=strdup(line);
  while (val--) { s=f; while (s) { strcpy(line,s->string); s=s->next; ParseLine(); } }
  strcpy(line,ml); listmacro=olistmacro; donotlist=1;
}

void InsertDirectives() {
  dirtab.insertd("assert",dirASSERT);
  dirtab.insertd("byte",dirBYTE);
  dirtab.insertd("abyte",dirABYTE);
  dirtab.insertd("abytec",dirABYTEC);
  dirtab.insertd("abytez",dirABYTEZ);
  dirtab.insertd("word",dirWORD);
  dirtab.insertd("block",dirBLOCK);
  dirtab.insertd("dword",dirDWORD);
  dirtab.insertd("d24",dirD24);
  dirtab.insertd("org",dirORG);
  dirtab.insertd("header",dirHEADER);
  dirtab.insertd("map",dirMAP);
  dirtab.insertd("align",dirALIGN);
  dirtab.insertd("module",dirMODULE);
  dirtab.insertd("z80",dirZ80);
  dirtab.insertd("arm",dirARM);
  dirtab.insertd("thumb",dirTHUMB);
  dirtab.insertd("size",dirSIZE);
  dirtab.insertd("textarea",dirTEXTAREA);
  dirtab.insertd("phase",dirTEXTAREA);
  dirtab.insertd("msx",dirZ80);
  dirtab.insertd("else",dirELSE);
  dirtab.insertd("export",dirEXPORT);
  dirtab.insertd("end",dirEND);
  dirtab.insertd("include",dirINCLUDE);
  dirtab.insertd("incbin",dirINCBIN);
  dirtab.insertd("fsize",dirFSIZE);
  dirtab.insertd("if",dirIF);
  dirtab.insertd("output",dirOUTPUT);
  dirtab.insertd("define",dirDEFINE);
  dirtab.insertd("ifdef",dirIFDEF);
  dirtab.insertd("ifndef",dirIFNDEF);
  dirtab.insertd("macro",dirMACRO);
  dirtab.insertd("struct",dirSTRUCT);
  dirtab.insertd("dc",dirDC);
  dirtab.insertd("dz",dirDZ);
  dirtab.insertd("db",dirBYTE);
  dirtab.insertd("dw",dirWORD);
  dirtab.insertd("ds",dirBLOCK);
  dirtab.insertd("dd",dirDWORD);
  dirtab.insertd("dm",dirBYTE);
  dirtab.insertd("dbl",dirBYTEL);
  dirtab.insertd("dbr",dirBYTER);
  dirtab.insertd("dbs",dirBYTES);
  dirtab.insertd("dp2",dirPIXEL2);
  dirtab.insertd("dp4",dirPIXEL4);
  dirtab.insertd("dp16",dirPIXEL16);
  dirtab.insertd("defb",dirBYTE);
  dirtab.insertd("defw",dirWORD);
  dirtab.insertd("defs",dirBLOCK);
  dirtab.insertd("defd",dirDWORD);
  dirtab.insertd("defm",dirBYTE);
  dirtab.insertd("endmod",dirENDMODULE);
  dirtab.insertd("endmodule",dirENDMODULE);
  dirtab.insertd("endmap",dirENDMAP);
  dirtab.insertd("rept",dirREPT);
  dirtab.insertd("fpos",dirFORG);
//  dirtab.insertd("bind",dirBIND);
  dirtab.insertd("endif",dirENDIF);
  dirtab.insertd("endt",dirENDTEXTAREA);
  dirtab.insertd("dephase",dirENDTEXTAREA);
  dirtab.insertd("endm",dirENDM);
  dirtab.insertd("ends",dirENDS);
#ifdef SECTIONS
  dirtab.insertd("code",dirTEXT);
  dirtab.insertd("data",dirDATA);
  dirtab.insertd("text",dirTEXT);
  dirtab.insertd("pool",dirPOOL);
#endif
}
//eof direct.cpp
