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

// sjasm.cpp

#include "sjasm.h"

char destfilename[LINEMAX],listfilename[LINEMAX],expfilename[LINEMAX],sourcefilename[LINEMAX],symfilename[LINEMAX];

char filename[LINEMAX],*lp,line[LINEMAX],temp[LINEMAX],*tp,pline[LINEMAX*2],eline[LINEMAX*2],*bp;

int pass=0,labelnotfound,nerror=0,include=-1,running,labellisting=0,listfile=1,donotlist,listdata,listmacro;
int macronummer,lijst,reglenwidth,synerr=1,symfile=0;
aint adres,mapadr,gcurlin,lcurlin=0,curlin,destlen,size=(aint)-1,preverror=(aint)-1,maxlin=0,comlin;
#ifdef METARM
cpus cpu;
#endif
char *huidigzoekpad;
int endadres=0;
unsigned char exoshdrtype=0;
bool relocpass=false;

void (*piCPUp)(void);

#ifdef SECTIONS
sections section;
#endif

char *modlabp,*vorlabp=0,*macrolabp;

stringlst *lijstp;
labtabcls labtab;
loklabtabcls loklabtab;
definetabcls definetab;
macdefinetabcls macdeftab;
macrotabcls macrotab;
structtabcls structtab;
adrlst *maplstp=0;
stringlst *modlstp=0,*dirlstp=0;
#ifdef SECTIONS
pooldatacls pooldata;
pooltabcls pooltab;
#endif

void InitPass(int p) {
#ifdef SECTIONS
  section=TEXT;
#endif
  reglenwidth=1;
  if (maxlin>9) reglenwidth=2;
  if (maxlin>99) reglenwidth=3;
  if (maxlin>999) reglenwidth=4;
  if (maxlin>9999) reglenwidth=5;
  if (maxlin>99999) reglenwidth=6;
  if (maxlin>999999) reglenwidth=7;
  modlabp=0; vorlabp=strdup("_"); macrolabp=0; listmacro=0;
  pass=p; adres=mapadr=0; running=1; gcurlin=lcurlin=curlin=0;
  eadres=0; epadres=0; macronummer=0; lijst=0; comlin=0;
  modlstp=0;
#ifdef METARM
  cpu=Z80; piCPUp=piZ80;
#endif
  structtab.init();
  macrotab.init();
//definetab.init();
  macdeftab.init();
}

void getOptions(char **&argv,int &i) {
  const char *p;
  char c;
  while (argv[i] && *argv[i]=='-') {
    p=argv[i++]+1;
    do {
      c=*p++; c=tolower(c);
      switch (c) {
      case 'q': listfile=0; break;
      case 's': symfile=1; break;
      case 'l': labellisting=1; break;
      case 'i': dirlstp=new stringlst(p,dirlstp); p=""; break;
      case 'd':
      case 'e':
        {
          const char  *value = strchr(p, '=');
          char    *endp = (char *) p;
          if (!value)
            value = "1";
          else
            value++;
          char    *name = getid(endp);
          if (!name || (*endp != '\0' && *endp != '='))
            error("Invalid name", p, FATAL);
          if (c == 'd')
            definetab.add(name, value);
          else if (!labtab.insert(name, strtol(value, &endp, 0)))
            error("Duplicate label", name, FATAL);
          free(name);
          p = "";
        }
        break;
      default:
        fprintf(stderr, "Unrecognised option: %c\n", int(c));
        break;
      }
    } while (*p);
  }
}

void relocfile();

int main(int argc, char *argv[]) {
  char zoekpad[MAX_PATH];
  char *p;
  int i=1;

  printf("SjASM Z80 Assembler v0.39g6 - www.xl2s.tk\n");
  sourcefilename[0]=destfilename[0]=listfilename[0]=expfilename[0]=0;
  if (argc==1) {
    printf("Copyright 2006 Sjoerd Mastijn\n");
    printf("\nUsage:\nsjasm [-options] sourcefile [targetfile [listfile [exportfile]]]\n");
    printf("\nOption flags as follows:\n");
    printf("  -l        Label table in listing\n");
    printf("  -s        Generate .SYM symbol file\n");
    printf("  -q        No listing\n");
    printf("  -i<path>  Includepath\n");
    printf("  -Dname=value  Define identifier\n");
    printf("  -Ename=value  Define label\n");
    return 1;
  }

  GetCurrentDirectory(MAX_PATH,zoekpad);
  huidigzoekpad=zoekpad;

  definetab.init();
  getOptions(argv,i); if (argv[i]) strcpy(sourcefilename,argv[i++]);
  getOptions(argv,i); if (argv[i]) strcpy(destfilename,argv[i++]);
  getOptions(argv,i); if (argv[i]) strcpy(listfilename,argv[i++]);
  getOptions(argv,i); if (argv[i]) strcpy(expfilename,argv[i++]);
  getOptions(argv,i);

  if (!sourcefilename[0]) error("No inputfile",0,FATAL);
  if (!destfilename[0]) {
    strcpy(destfilename,sourcefilename);
    if (!(p=strchr(destfilename,'.'))) p=destfilename; else *p=0;
    strcat(p,".out");
  }
  if (!listfilename[0]) {
    strcpy(listfilename,sourcefilename);
    if (!(p=strchr(listfilename,'.'))) p=listfilename; else *p=0;
    strcat(p,".lst");
  }
  strcpy(symfilename,expfilename);
  if (!expfilename[0]) {
    strcpy(expfilename,sourcefilename);
    if (!(p=strchr(expfilename,'.'))) p=expfilename; else *p=0;
    strcat(p,".exp");
  }
  if (!symfilename[0]) {
    strcpy(symfilename,sourcefilename);
    if (!(p=strchr(symfilename,'.'))) p=symfilename; else *p=0;
    strcat(p,".sym");
  }

  Initpi();

  InitPass(1); OpenList(); OpenFile(sourcefilename); endadres=adres;

  printf("Pass 1 complete (%d errors)\n", nerror);

  InitPass(2); OpenDest(); OpenFile(sourcefilename);

  if (labellisting) labtab.dump();

  printf("Pass 2 complete\n");

  if (exoshdrtype && (exoshdrtype<5 || exoshdrtype>6) && !nerror) relocfile();

  Close();

  if (nerror) remove(destfilename);
  else if (symfile) labtab.dumpsym();

  printf("Errors: %d\n", nerror); fflush(stdout);

  return (nerror!=0);
}

extern void printhex8(char *&p, aint h);
extern FILE *output;

void relocfile() {
  const int reloc_offs=6037;
  int err_offs=0;
  const char *errmsg=0;
  unsigned char *buf=0;
  size_t fsize;
  do {
    relocpass=true; Close(); listfile=listdata=listmacro=0;
    InitPass(1); adres=reloc_offs; OpenFile(sourcefilename);
    InitPass(2); adres=reloc_offs; OpenFile(sourcefilename);
    Close(); relocpass=false;
    if (!(output=freopen(destfilename,"rb",output)))
      error("Error reopening output file",0,FATAL);
    if (size!=-1) endadres=int(size>>1);
    fsize=size_t(destlen);
    if (fsize<2 || fsize>(0xe380<<1) || endadres<1 || endadres>0xe380)
      error("Invalid relocatable module size",0,FATAL);
    if (!(buf = (unsigned char *) malloc((fsize*3+64)*sizeof(unsigned char))))
      error("Error allocating memory",0,FATAL);
    if (fread(buf,sizeof(unsigned char),fsize,output) != fsize)
      { free(buf); error("Error reading output file",0,FATAL); }
    if (!(output=freopen(destfilename,"wb",output)))
      { free(buf); error("Error reopening output file",0,FATAL); }
    unsigned char *buf2=buf+fsize;
    size_t fsize2=16;
    for (int i=0; i<26; i++) buf2[i]=0;
    buf2[1]=exoshdrtype;
    if (exoshdrtype==2) {
      buf2[4]=0xff;
      buf2[5]=0xff;
    }
    else if (exoshdrtype==4) {
      buf2[16]=0x0a; buf2[17]=100; buf2[20]=0x60; buf2[22]=0xc2;
      buf2[23]=endadres&0xff; buf2[24]=(endadres>>8)&0xff;
      fsize2=26;
    }
    unsigned int sr=0,srbits=0;
    for (int i=0; i<endadres; i++) {
      err_offs=i;
      if (size_t(endadres+i) >= fsize)
        { errmsg="Unexpected end of output"; break; }
      if (buf[i]!=buf[endadres+i]) {
        if (size_t(endadres+i+1) >= fsize)
          { errmsg="Unexpected end of output"; break; }
        int a1 = int(buf[i]) | (int(buf[i+1])<<8);
        int a2 = int(buf[long(endadres)+i]) | (int(buf[long(endadres)+i+1])<<8);
        if ((a2-a1)!=reloc_offs || exoshdrtype==4)
          { errmsg="Invalid relocatable code"; break; }
        sr = (sr<<11)|0x400|((a1-i)&0xff); sr = (sr<<8)|(((a1-i)>>8)&0xff);
        srbits = srbits+19;
        i++;
      }
      else if (exoshdrtype==4) {
        if ((i&15)==0) {
          buf2[fsize2+1]=((i>>4)+101)&0xff;
          buf2[fsize2+2]=(((i>>4)+101)>>8)&0xff;
          buf2[fsize2+3]=0;
          unsigned char *p=buf2+(fsize2+4);
          if (!i) { strcpy((char *) p,"\140\011!C\023DHEX$\010\200"); p=p+12; }
          else { strcpy((char *) p,"\140\011\023DHEX$\010\200"); p=p+10; }
          unsigned char l=((endadres-i)<16 ? (endadres-i):16)*3-1;
          buf2[fsize2] = (unsigned char) (p-(buf2+fsize2)) + (l+3);
          *p=l;
          fsize2=p+1-buf2;
        }
        else buf2[fsize2++]=0x2c;
        char *p = (char *) (buf2+fsize2); printhex8(p,buf[i]); fsize2=fsize2+2;
        if ((i&15)==15 || (i+1)>=endadres)
          { buf2[fsize2++]=0x09; buf2[fsize2++]=0x00; }
      }
      else
        { sr = (sr<<9)|buf[i]; srbits = srbits+9; }
      if ((i+1)>=endadres) {
        if (exoshdrtype==4) buf2[fsize2++]=0;
        else { sr = (sr<<3)|6; srbits = srbits+3; }
      }
      while (srbits>=8)
        { buf2[fsize2++] = (sr>>(srbits-8))&0xff; srbits = srbits-8; }
    }
    if (errmsg) break;
    if (srbits) buf2[fsize2++] = (sr<<(8-srbits))&0xff;
    buf2[2]=(fsize2-16)&0xff;
    buf2[3]=((fsize2-16)>>8)&0xff;
    if (exoshdrtype==4) {
      for (int i=0; i<16; i++) buf2[fsize2++]=0;
      buf2[fsize2-15]=0x0a;
    }
    if (fwrite(buf2,sizeof(unsigned char),fsize2,output) != fsize2 ||
        fflush(output)!=0) {
      free(buf); error("Write error (disk full?)",0,FATAL); return;
    }
  } while (false);
  if (buf) free(buf);
  if (errmsg) {
    char tmpbuf[128];
    sprintf(tmpbuf,"%s at output offset 0x%08X",errmsg,(unsigned int) err_offs);
    error(tmpbuf,0,FATAL);
  }
}
//eof sjasm.cpp
