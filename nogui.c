#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define INT int

#define LINEMAX 256
typedef char String[LINEMAX];
String Buffer;

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#define FULLARC (360 * 64)


#define WINDOW_WIDTH 820

/* Models */

#define PUMA    0
#define SAM     1
#define CAT     2
#define PLASIM  3
#define MODELS  4

/* Resolutions */

#define RES_T15 0
#define RES_T21 1
#define RES_T31 2
#define RES_T42 3
#define RES_T63 4
#define RESOLUTIONS 5

int PlasimSteps[RESOLUTIONS] =
{
   60,  // T15
   45,  // T21
   30,  // T31
   20,  // T42
   10   // T63
};

int PumaSteps[RESOLUTIONS] =
{
   60,  // T15
   60,  // T21
   45,  // T31
   30,  // T42
   15   // T63
};

int ResLat[RESOLUTIONS] =
{
   24,
   32,
   48,
   64,
   96
};

int Resolution = -1;

char *ShortModelName[MODELS] =
{
   "puma",
   "sam",
   "cat",
   "plasim"
};

char *FullModelName[MODELS] =
{
   "PUMA",
   "SAM",
   "CAT",
   "Planet Simulator"
};

int Model = PLASIM;

#define ALL    -1
#define EARTH   0
#define MARS    1
#define EXO     2
#define PLANETS 3

char *PlanetName[PLANETS] =
{
   "Earth",
   "Mars",
   "Exo"
};

int Planet = EARTH;

double *OroEarth;  // Earth Orography T42 (128 x 64)
double *OroMars;   // Mars  Orography T42 (128 x 64)
double *OroPrep;   // Preprocessed Orography

double RevGra = 1.0 / 9.81;

/* Object types */

#define SEL_TEXT   1
#define SEL_CHECK  2
#define SEL_INT    3
#define SEL_REAL   4
#define SEL_TEVA   5
#define SEL_PLANET 6

struct ItemStruct
{
   char   list[80];      // Name of namelist
   char   name[80];      // Name of variable
   char   text[80];      // Text    value
   float  pvec[PLANETS]; // Planetary parameter
   double rval;          // Real    value
   int    ival;          // Integer value
   int    model;         // Model
   int    flag;          // Flag
};

struct SelStruct
{
   struct SelStruct  *Next; // Link to next Sel
   struct SelStruct  *Prev; // Link to previous Sel
   struct ItemStruct *Item; // Link to item
   int type;                // Type (TEXT,CHECK,INT,REAL)
   int x;                   // x coordinate of box
   int y;                   // y coordinate of box
   int h;                   // height of box
   int w;                   // width of box
   int xo;                  // x offset
   int xt;                  // x coordinate for text
   int yt;                  // y coordinate for text
   int lt;                  // length of text
   int iv;                  // integer value of box content
   int div;                 // default integer value
   int no;                  // 1: not selectable
   int teco;                // text colour
   int edco;                // edit column (cursor)
   int hide;                // 1: don't show
   float fv;                // floating value of box content
   float dfv;               // default floating value
   float fpl[PLANETS];      // planet dependent floats
   int   *piv;              // pointer to linked integer variable
   float *pfv;              // pointer to linked float   variable
   char text[80];           // text to display
   char teva[16];           // box content
};

struct SelStruct SelStart;
struct SelStruct SelModels[MODELS];
struct SelStruct *CursorSel;
struct SelStruct *ComEnd;
struct SelStruct *SelAno;
struct SelStruct *SelTgr;
struct SelStruct *SelOro;
struct SelStruct *SelMod;
struct SelStruct *SelRes; 
struct SelStruct *SelOce;
struct SelStruct *SelLsg;
struct SelStruct *SelIce;
struct SelStruct *SelAnn;
struct SelStruct *SelPlanet[PLANETS];
struct SelStruct *SelSYear;
struct SelStruct *SelCPU;
struct SelStruct *SelMulti;
struct SelStruct *SelLat2;


// Title lines for the entry of resolution

char *DimText1;
char *DimText2;
char *DimText3;

#define DIMLOGO 4

struct SymbolStruct 
{
   int     x;     // x pos
   int     y;     // y pos
   int     w;     // width
   int     h;     // height
   int     b;     // background color
   int     f;     // foreground color
   char  **i;     // image
   char   *d;     // image data
   char   t[2][80]; // caption
} Logo[DIMLOGO];
  
int Logos;

struct ColorStrip
{
   double  Lo;
   double  Hi;
   char *Name;
   unsigned long pixel;
};

unsigned long WinBG = 0; // Black background
unsigned long TextC = 0; // Text color
unsigned long HeadC = 0; // Heading color

#define DIMFRAME 3

int Frames;

struct FrameStruct 
{
   void (*Action)();
   int    x;        // x pos
   int    y;        // y pos
   int    w;        // width
   int    h;        // height
   int    b;        // background color
   int    f;        // foreground color
   int    xs;       // x pos  selection
   int    ys;       // y pos  selection
   int    ws;       // width  selection
   int    hs;       // height selection
   char   t[3][80]; // text
} Frame[DIMFRAME];
  
int FrameNo;
int Buttons;
int Button1Down = 1;

#define DIMBUTTON 5

struct ButtonStruct 
{
   void (*Action)();
   int    x;        // x pos
   int    y;        // y pos
   int    w;        // width
   int    h;        // height
   int    b;        // background color
   int    f;        // foreground color
   char   t[3][80]; // text
} Button[DIMBUTTON];
  
int Buttons;


/* Coordinates */

int opbox_x;
int opbox_y;
int opbox_w;
int opbox_h;

int nlbox_x;
int nlbox_y;
int nlbox_w;
int nlbox_h;

int nlpos_x;
int nledi_x;

int sfbox_x;
int sfbox_y;
int sfbox_w;
int sfbox_h;
int sfbox_b;

int bubox_x;
int bubox_y;
int bubox_w;
int bubox_h;

int CursorCol;
int LastSel;
int EdiKeyCode;

#define NL_MAX_ITEMS 100

int NL_items = 0;
struct ItemStruct NL_list[NL_MAX_ITEMS];

int BigEndian;
int Oce;
int Ice;
int Lsg;
int SimStart;
int SimYears;
int nreadsr;
int noutput;
int ndebug;
int nprec;
int ngui;
int noro = 1;
int OroAno;
int OroClear;
int OroAqua;
int TgrAno;
int nac;
int Preprocessed;
int SAMindex;
int ScreenHeight;
int Expert = 0;
int CatEnabled;
int SamEnabled;
int LsgEnabled;
int ModeRadiusSq;
int ForceRebuild;
int dxsh;
int dxs2;
int Yoden;                // PUMA setup for Yoden experiment
int CatSim = 51;          // simulation setup number for CAT

/* Special parameter */

int Latitudes       = 32; // Number of latitudes in atmosphere
int Latitude2       = 32; // Number of latitudes for 2nd. instance
int Levels          = 10; // Number of levels in atmosphere
int Cores           =  1; // Number of cores for parallel version
int Multirun        =  1; // Number of coupled runs
int MultirunEnabled =  0; // Multirun module present?
int Truncation      = 21; // Spectral truncation computed from Latitudes

unsigned Seed;

char cfg_file[256]      = "most_last_used.cfg";
char exec_name[256]     = "most_puma.x";
char exec_nam2[256]     = "most_puma.x";  // 2nd. instance
char exec_ppp[256]      = "most_ppp.x";
char oro_name[256]      = "N064_surf_0129.sra"; // Orography in T42 resolution
char namelist_name[256] = "puma_namelist";
char diag_name[256]     = "puma_diag";
char outp_name[256]     = "puma_output";
char build_name[256]    = "most_puma_build";
char build_ppp[256]     = "most_ppp_build";
char run_name[256]      = "most_puma_run";
char run_ppp[256]       = "most_ppp_run";
char res_name[256]      = "resmod.f90";
char hostname[256]      = "localhost";
char mpirun[256]        = "mpirun";       // "openmpirun" for Open MPI

int ScreenN;
time_t CurrentDate;

unsigned int ScreenW,ScreenH;
int SmallScreen = 1;

static char *progname = "Most";

int DimX;
int DimY;
int DimTr = 21;
int DimSH = 11 * 23;
int DimSE = 11 * 23 + 2;
int ScreenD;
int OffX;
int OffY = 0;
int WinXSize;
int WinYSize;
int WhitePix;
int BlackPix;

int *Ampli;
int *ModeX;
int *ModeY;
int *ModeM;
int *ModeN;

int count;
char ModFontName[80] = "-misc-fixed-bold-r-normal--15-*-*-*-*-*-*-*";
char FixFontName[80] = "-misc-fixed-bold-r-normal--15-*-*-*-*-*-*-*";
char BigFontName[80] = "-misc-fixed-medium-r-normal--20-*-*-*-*-*-*-*";
//char ModFontName[80] = "9x15bold";
//char FixFontName[80] = "9x15bold";
//char BigFontName[80] = "10x20";
int ModFontHeight;
int FixFontHeight;
int BigFontHeight;
int ModFontWidth;
int FixFontWidth;
int BigFontWidth;
int ModFontAscent;
int FixFontAscent;
int BigFontAscent;
int EdiFirstKey;
int EdiLastKey;
int EdiSymsPerKey;
int NumLockMask;
int ModeSwitchMask;

int Debug = 0;

char *display_name = NULL;

char *mona[12] =
{
   "Jan","Feb","Mar",
   "Apr","May","Jun",
   "Jul","Aug","Sep",
   "Oct","Nov","Dec"
};

char datch[32];

struct MapImageStruct
{
   char   *d; // Bitmap data
   int     w; // Image width
   int     h; // Image height
   int     f; // Rotation factor
   float   l; // Reference longitude
   float   r; // Rotation speed [deg/step]
};

struct MapImageStruct MapHRE; // Hires (2560x1280) Earth image
struct MapImageStruct MapHRM; // Hires (         ) Mars  image
struct MapImageStruct MapLRE; // Lores azimuthal   Earth image
struct MapImageStruct MapLRM; // Lores azimuthal   Mars  image
struct MapImageStruct MapLRK; // Lores azimuthal   Kepler-16
struct MapImageStruct MapLRL; // Lores azimuthal   Kepler-186b

/* Isoarea data */

#define TOLELO 0x0001
#define TOLEHI 0x0002
#define TORILO 0x0004
#define TORIHI 0x0008
#define BOLELO 0x0010
#define BOLEHI 0x0020
#define BORILO 0x0040
#define BORIHI 0x0080
#define TOLEIN 0x0100
#define TOREIN 0x0200
#define BOLEIN 0x0400
#define BOREIN 0x0800

#define IPX(l,m,h) (VGAX * (x + (l-m) / (l-h)))
#define IPY(l,m,h) (VGAY * (y + (l-m) / (l-h)))

int nlon = 128;
int nlat =  64;
int nlev =   5;

int *Flag;

double VGAX;
double VGAY;

/* Orography units are [m2/s2] */

struct ColorStrip OroStrip[] =
{
   {-99999.0,   80.0,"DarkBlue"},
   {    80.0,  800.0,"green"},
   {   800.0, 1600.0,"brown"},
   {  1600.0, 2400.0,"orange"},
   {  2400.0,99999.0,"white"},
   {     0.0,     0.0,NULL}
};

struct ColorStrip OroMarsStrip[] =
{
   {-99999.0,-16000.0,"yellow"},
   {-16000.0,-12000.0,"RoyalBlue4"},
   {-12000.0, -8000.0,"RoyalBlue3"},
   { -8000.0, -4000.0,"RoyalBlue2"},
   { -4000.0,     0.0,"RoyalBlue1"},
   {     0.0,  4000.0,"dark green"},
   {  4000.0,  8000.0,"light green"},
   {  8000.0, 12000.0,"brown"},
   { 12000.0, 16000.0,"orange"},
   { 16000.0, 99999.0,"white"},
   {     0.0,     0.0,NULL}
};

struct ColorStrip GibbStrip[] =
{
   {-99999.0,    0.0,"DarkBlue"},
   {     0.0,99999.0,"green"},
   {     0.0,     0.0,NULL}
};


struct ColorStrip TStrip[] =
{
   {-99.0,-15.0,"RoyalBlue4"},
   {-15.0,-20.0,"RoyalBlue3"},
   {-10.0, -5.0,"RoyalBlue2"},
   { -5.0, -0.1,"RoyalBlue1"},
   { -0.1,  0.1,"yellow"},
   {  0.1,  5.0,"IndianRed1"},
   {  5.0, 10.0,"IndianRed2"},
   { 10.0, 15.0,"IndianRed3"},
   { 15.0, 20.0,"IndianRed4"},
   { 20.0,999.0,"red"},
   {  0.0,  0.0,NULL}
};

void FormatReal(float fv, char *text)
{
   if (fv == 0.0                      ||
      (fv >   0.001 && fv < 99999.0)  ||
      (fv > -9999.0 && fv <  -0.001))
   {
      sprintf(text,"%11.4f",fv);
           if (!strcmp(text+8,"000")) text[ 8] = 0;
      else if (!strcmp(text+9,"00" )) text[ 9] = 0;
      else if (text[10] == '0')       text[10] = 0;
   }
   else
   {
      sprintf(text,"%11.4e",fv);
   }
}


void FinishLine(void)
{
   char text[80];
   int  OldCores;

   OldCores = Cores;

   if (CursorSel == NULL) return;
   if (CursorSel->type == SEL_INT)
   {
      CursorSel->iv = atoi(CursorSel->teva);
      sprintf(text,"%10d",CursorSel->iv);
      strcpy(CursorSel->teva,text+10-CursorSel->edco);
      if (CursorSel->piv) *CursorSel->piv = CursorSel->iv;
   }
   if (CursorSel->type == SEL_REAL)
   {
      CursorSel->fv = atof(CursorSel->teva);
      FormatReal(CursorSel->fv,text);
      strcpy(CursorSel->teva,text);
   }

/*
   if (CursorSel == SelMulti)  // Enable or disable Lat2
   {
      SelLat2->no = (SelMulti->iv != 2) ;
   }
*/
   if (OldCores == 1 && Cores  > 1) ForceRebuild = 1;
   if (OldCores  > 1 && Cores == 1) ForceRebuild = 1;
   if (Model == PLASIM && OldCores != Cores) ForceRebuild = 1;
}

void WriteNamelistFile(char *nl,  int instance)
{
   char *shomo = ShortModelName[Model];
   struct SelStruct *Sel;
   char fn[256];
   FILE *fp;

   if (Multirun > 1)
      sprintf(fn,"%s/run/%s_namelist_%2.2d",shomo,nl,instance);
   else
      sprintf(fn,"%s/run/%s_namelist",shomo,nl);

   fp = fopen(fn,"w");
   if (fp == NULL)
   {
      printf("Could not open file <%s> for writing\n",fn);
      return; /* Failure */
   }

   // Write namelist file

   fprintf(fp," &%s_nl\n",nl);

   // Write special parameters

   if (!strcmp(nl,"icemod"))
   {
      fprintf(fp," %-12s=%6d\n","NICE",Ice);
   }
   if (!strcmp(nl,"oceanmod"))
   {
      fprintf(fp," %-12s=%6d\n","NOCEAN",Oce);
//FL0318
      if (Lsg)
      {
         fprintf(fp," %-12s=%6d\n","NLSG"  ,Lsg);
      }
   }
   if (!strcmp(nl,"plasim"))
   {
      fprintf(fp," %-12s=%6d\n","NOUTPUT",noutput);
      fprintf(fp," %-12s=%6d\n","NGUI"  ,ngui);
      fprintf(fp," %-12s=%6d\n","N_START_YEAR",SimStart);
      if (Lsg)        fprintf(fp," %-12s=%6d\n","N_DAYS_PER_YEAR",360);
      else if (Planet != MARS) fprintf(fp," %-12s=%6d\n","N_DAYS_PER_YEAR",365);
      if (ngui) fprintf(fp," %-12s=%6d\n","N_RUN_YEARS",SimYears);
      else      fprintf(fp," %-12s=%6d\n","N_RUN_YEARS",1);
      fprintf(fp," %-12s=%6d\n","N_RUN_MONTHS",0);
      fprintf(fp," %-12s=%6d\n","N_RUN_DAYS",0);
   }
   for (Sel = ComEnd->Next ; Sel ; Sel = Sel->Next)
   {
      if (Sel->Item && !strcasecmp(Sel->Item->list,nl))
      {    
         if (Sel->type == SEL_TEVA)
            fprintf(fp," %-12s=%c%s%c\n",Sel->text,'\"',Sel->teva,'\"');
         else
            fprintf(fp," %-12s=%s\n",Sel->text,Sel->teva);
      }    
   }
   fprintf(fp," /END\n");
   fclose(fp);
}

void WritePlasimNamelist(void)
{
   int imr;

   FinishLine();

   for (imr = 0 ; imr < Multirun ; ++imr)
   {
      WriteNamelistFile("fluxmod" ,imr);
      WriteNamelistFile("icemod"  ,imr);
      WriteNamelistFile("landmod" ,imr);
      WriteNamelistFile("vegmod"  ,imr);
      WriteNamelistFile("miscmod" ,imr);
      WriteNamelistFile("oceanmod",imr);
      WriteNamelistFile("planet"  ,imr);
      WriteNamelistFile("plasim"  ,imr);
      WriteNamelistFile("radmod"  ,imr);
      WriteNamelistFile("rainmod" ,imr);
      WriteNamelistFile("seamod"  ,imr);
      WriteNamelistFile("surfmod" ,imr);
   }
}

int WriteRunScript(int model)
{
   int i;
   int porm;
   FILE *fp;
   char command[256];
   char run[256];

   strcpy(exec_nam2,exec_name); // Duplicate exec name

   if (model == CAT) // Add Dimensions
   {
      sprintf(exec_name+strlen(exec_name)," %d %d",Latitudes,Latitude2);
   }

   if (model == PUMA) // Add Latitudes and Levels as arguments
   {
      sprintf(exec_name+strlen(exec_name)," %d %d",Latitudes,Levels);
      sprintf(exec_nam2+strlen(exec_nam2)," %d %d",Latitude2,Levels);
   }

   if (model == SAM) // Add Latitudes as arguments
   {
      sprintf(exec_name+strlen(exec_name)," %d",Latitudes);
      sprintf(exec_nam2+strlen(exec_nam2)," %d",Latitude2);
   }

   // porm is for Parallel OR Multiprocessing

   porm = MAX(Cores,Multirun);

   strcpy(run,ShortModelName[model]);
   strcat(run,"/run/");
   strcat(run,run_name);

   fp = fopen(run,"w");
   if (fp == NULL)
   {
      printf("Could not open file <%s> for writing\n",run);
      return 0; /* Failure */
   }
   fputs("#!/bin/bash\n",fp);
   fprintf(fp,"# run-script generated by Most %s",ctime(&CurrentDate));
   fputs("EXP=MOST    # Name your experiment here\n",fp);
   fprintf(fp,"[ $# == 1 ] && cd $1\n");
   fprintf(fp,"rm -f %s_restart\n",ShortModelName[model]);
   fputs("rm -f Abort_Message\n",fp);
   fputs("YEAR=0\n",fp);
   fprintf(fp,"YEARS=%d\n",SimYears);
   if (Multirun > 1) fprintf(fp,"INSTANCES=%d\n",Multirun);
   if (ngui) fputs("# Remove '#' from 'while' and 'end' lines for restart loop\n",fp);
   if (ngui) fputs("# ",fp); /* deactivate loop for GUI case */
   fputs("while [ $YEAR -lt $YEARS ]\n",fp);
   if (ngui) fputs("# ",fp); /* deactivate loop for GUI case */
   fputs("do\n",fp);
   fputs("   YEAR=`expr $YEAR + 1`\n",fp);
   fputs("   DATANAME=`printf '%s.%03d' $EXP $YEAR`\n",fp);
   fputs("   DIAGNAME=`printf '%s_DIAG.%03d' $EXP $YEAR`\n",fp);
   fputs("   RESTNAME=`printf '%s_REST.%03d' $EXP $YEAR`\n",fp);
   if (porm < 2)
   {
      fprintf(fp,"   ./%s\n",exec_name);
   }
   else 
   {
      if (Multirun > 1) 
      {    
         fprintf(fp,"   %s",mpirun);
         fprintf(fp," -np 1 %s : -np 1 %s\n",exec_name,exec_nam2);
      }    
      else 
      {    
         fprintf(fp,"   %s -np %d %s\n",mpirun,porm,exec_name);
      }    
   }
   fputs("   [ -e Abort_Message ] && exit 1\n",fp);
   if (Multirun > 1)
   {
      fputs("   INST=0\n",fp);
      fputs("   while [ $INST -lt $INSTANCES ]\n   do\n",fp);
      fputs("      EXT=`printf '%02d' $INST`\n",fp);
      fprintf(fp,"      [ -e %s_$EXT ] && mv %s_$EXT ${EXT}_$DATANAME\n",
              outp_name,outp_name);
      fprintf(fp,"      mv %s_$EXT   ${EXT}_$DIAGNAME\n",diag_name);
      fprintf(fp,"      cp %s_status_$EXT %s_restart_$EXT\n",
              ShortModelName[model],ShortModelName[model]);
      fprintf(fp,"      mv %s_status_$EXT ${EXT}_$RESTNAME\n",ShortModelName[model]);
      fputs("      INST=`expr $INST + 1`\n",fp);
      fputs("   done\n",fp);
   }
   else
   {
      fprintf(fp,"   [ -e %s ] && mv %s $DATANAME\n",outp_name,outp_name);
      fprintf(fp,"   [ -e %s ] && mv %s $DIAGNAME\n",diag_name,diag_name);
      fprintf(fp,"   [ -e %s_status ] && cp %s_status %s_restart\n",
              ShortModelName[model],ShortModelName[model],ShortModelName[model]);
      fprintf(fp,"   [ -e %s_status ] && mv %s_status $RESTNAME\n",
              ShortModelName[model],ShortModelName[model]);
   }
   if (ngui) fputs("# ",fp); /* deactivate loop for GUI case */
   fputs("done\n",fp);
   fclose(fp);
   sprintf(command,"chmod a+x %s",run);
   system(command);
   return 1; /* Success */
}

void GenerateNames(void)
{
   Truncation = (2 * Latitudes - 1) / 3;
   sprintf(namelist_name,"%s_namelist",ShortModelName[Model]);
   if (Model == CAT)
   {
      if (Cores < 2) strcpy(exec_name,"most_cat.x");
      else           strcpy(exec_name,"most_cat_mpi.x");
   }
   else if (Model == PUMA)
   {
      if (Cores < 2) strcpy(exec_name,"most_puma.x");
      else           strcpy(exec_name,"most_puma_mpi.x");
   }
   else if (Model == SAM)
   {
      if (Cores < 2) strcpy(exec_name,"most_sam.x");
      else           strcpy(exec_name,"most_sam_mpi.x");
   }
   else
      sprintf(exec_name,"most_%s_t%d_l%d_p%d.x",
      ShortModelName[Model],Truncation,Levels,Cores);

   sprintf(diag_name,"%s_diag",ShortModelName[Model]);
   sprintf(outp_name,"%s_output",ShortModelName[Model]);
   sprintf(build_name,"most_%s_build",ShortModelName[Model]);
   sprintf(build_ppp,"most_%s_build","ppp");
   sprintf(run_name,"most_%s_run",ShortModelName[Model]);
   if (Latitudes < 1000) sprintf(oro_name,"N%3.3d_surf_0129.sra",Latitudes);
   else                  sprintf(oro_name,"N%d_surf_0129.sra",Latitudes);
}

void WriteResmod(char *res)
{
   FILE *fp;
   int OldLat,OldLev,OldPro;
   char Line[80];

   // Read existing file if there and check for changes

   fp = fopen(res,"r");
   if (fp)
   {
      fgets(Line,80,fp); // header line 1
      fgets(Line,80,fp); // header line 2
      fgets(Line,80,fp);
      OldLat = atoi(Line+27);
      fgets(Line,80,fp);
      OldLev = atoi(Line+27);
      fgets(Line,80,fp);
      OldPro = atoi(Line+27);
      fclose(fp);
      if (OldLat == Latitudes && OldLev == Levels && OldPro == Cores) return;
   }
   fp = fopen(res,"w");
   if (fp == NULL)
   {
      printf("Could not open file <%s> for writing\n",res);
      exit(1); /* Failure */
   }
   fprintf(fp,"      module resmod ! generated by MoSt %s\n",ctime(&CurrentDate));
   fprintf(fp,"      parameter(NLAT_ATM = %d)\n",Latitudes);
   fprintf(fp,"      parameter(NLEV_ATM = %d)\n",Levels);
   fprintf(fp,"      parameter(NPRO_ATM = %d)\n",Cores);
   fprintf(fp,"      end module resmod\n");
   fclose(fp);
}

int CheckPlasimNamelist(void)
{
   int i,safe_mpstep;
   int *mpstep;
   double s;
   struct SelStruct *Sel;
   FILE *fp;

   FinishLine();

   for (Sel = &SelStart ; Sel ; Sel = Sel->Next)
   {
      if (Sel->piv) *Sel->piv = Sel->iv;
      if (!strcmp(Sel->text,"MPSTEP")) mpstep = &Sel->iv;
      if (!strcmp(Sel->text,"Orography"   )) nreadsr  = Sel->iv;
      if (!strcmp(Sel->text,"Annual cycle")) nac      = Sel->iv;
   }

   // Check for resolution defines as T value

   if (Resolution > 0) Latitudes = ResLat[Resolution];

   // LSG works currently only with T21 PlaSim
   
   if (Lsg)
   {
       Resolution = RES_T21;
       Latitudes = ResLat[RES_T21];
   }

   // Check # of latitudes for correct values (FFT requirements)

        if (Latitudes >=  64) Latitudes =  64; // T42
   else if (Latitudes >=  48) Latitudes =  48; // T31
   else if (Latitudes >=  32) Latitudes =  32; // T21
   else if (Latitudes >=   4) Latitudes =   4; // T2
   else if (Latitudes >=   2) Latitudes =   2; // T1

   // Set mpstep to a safe value

   if (Resolution > 0) *mpstep = PlasimSteps[Resolution];
   else
   {
      s = log(Latitudes / 32.0) / log(2.0);
      safe_mpstep = 45.0 / pow(3.0,s);
      if (*mpstep > safe_mpstep) *mpstep = safe_mpstep;
   }

   // Make sure # of CPU's is a power of 2 and in the proper range

   if (Cores > Latitudes) Cores = Latitudes;
   if (Cores <         1) Cores =         1;

   if (Latitudes % Cores != 0) Cores = 1;

   return 0; /* Success */
}

int Build(int model)
{
   int i,l,x,y,ierr;
   int porm;
   FILE *fp;
   char script_backup[256];
   char command[256];
   char message[256];
   char bld[256];
   char res[256];
   char *shomo;

   porm = MAX(Cores,Multirun);

   shomo = ShortModelName[model];
   strcpy(bld,shomo);
   strcat(bld,"/bld/");
   strcpy(res,bld);
   strcat(bld,build_name);
   strcat(res,res_name);
   strcpy(script_backup,bld);
   strcat(script_backup,".bak");
   rename(bld,script_backup);

   if (model == PLASIM) WriteResmod(res);

   fp = fopen(bld,"w");
   if (fp == NULL)
   {
      printf("Could not open file <%s> for writing\n",bld);
      return 0; /* Failure */
   }
   fputs("#!/bin/bash\n",fp);
   fprintf(fp,"# compile-script generated by Most %s",ctime(&CurrentDate));

   fputs("[ $# == 1 ] && cd $1\n",fp);
   if (ForceRebuild) fputs("rm -f *.x *.o *.mod\n",fp);
   fputs("cp -p ../src/* .\n",fp);
   if (ngui == 0) // Use stub routines and switch off X11 lib
   {
      putenv("GUIMOD=guimod_stub");
      putenv("PUMAX=pumax_stub");
      putenv("GUIX11=guix11_stub");
      putenv("GUILIB=");    
   }
   if (Planet == MARS) putenv("PLAMOD=p_mars");
   if (Planet == EXO ) putenv("PLAMOD=p_exo");
   if (Lsg)
   {
      fputs("cp -p ../../lsg/src/lsgmod.f90 .\n",fp);
      putenv("OCEANCOUP=cpl");
   }
   else
   {
      putenv("OCEANCOUP=cpl_stub");
   }
   if (Latitudes < 4) putenv("FFTMOD=fft991mod");
   if (porm > 1)
   {
      fputs("[ ! -e MPI ] && rm -f *.o *.mod *.x\n",fp);
      fputs("touch MPI\n",fp);
      fputs("cat ../../most_compiler_mpi",fp);
      if (Multirun > 1) putenv("MPIMOD=mpimod_multi");
   }
   else
   {
      fputs("[ -e MPI ] && rm -f *.o *.mod *.x MPI\n",fp);
      fputs("cat ../../most_compiler",fp);
   }
   if (ndebug) fputs(" ../../most_debug_options",fp);
   if (nprec)
   {
      fputs(" ../../most_precision_options",fp);
   }
   fprintf(fp," make_%s > makefile\n",shomo);
   fputs("make -e\n",fp);

   fprintf(fp,"[ $? == 0 ] && cp %s.x ../bin/%s\n",shomo,exec_name);

   fclose(fp);
   sprintf(command,"chmod a+x %s",bld);
   system(command);

   sprintf(message,"Building %s - wait a minute!",FullModelName[Model]);
   strcat(bld," ");
   strcat(bld,shomo);
   strcat(bld,"/bld");
   if (system(bld))
   {
      sleep(5);
      return 0; // error
   }
   sprintf(command,"cp %s/bin/%s %s/run\n",shomo,exec_name,shomo);
   system(command);

   // copy CAT simulation namelist to run directory

   if (model == CAT)
   {
      sprintf(command,"cp cat/dat/sim_%4.4d.nl cat/run/sim_namelist\n",CatSim);
      system(command);
   }

   // copy hires background bitmap for Earth or Mars

   if (model == PUMA || model == PLASIM )
   {
       if (model == PLASIM && Planet == MARS)
       {
          sprintf(command,"cp images/mars.bmp %s/run/map.bmp\n",shomo);
       system(command);
       }
       else if (model == PLASIM && Planet == EARTH)
       {
          sprintf(command,"cp images/earth.bmp %s/run/map.bmp\n",shomo);
          system(command);
       }
   }

   // copy surface data in matching resolution to run directory

   if (model == PLASIM)
   {
      if (Planet == MARS)
         sprintf(command,"cp plasim/dat/T%d_mars/* plasim/run/\n",Truncation);
      else if (Planet == EXO)
         sprintf(command,"cp plasim/dat/T%d_exo/* plasim/run/\n",Truncation);
      else
         sprintf(command,"cp plasim/dat/T%d/* plasim/run/\n",Truncation);
      system(command);

      if (Lsg) // copy data for LSG ocean model and select LSG GUI configuration
      {    
         sprintf(command,"cp lsg/dat/* %s/run\n",shomo);
         system(command);
         sprintf(command,"cp %s/dat/GUI_LSG.cfg %s/run/GUI.cfg\n",shomo,shomo);
         system(command);
      }
      else
      {
         sprintf(command,"cp %s/dat/GUI.cfg %s/run\n",shomo,shomo);
         system(command);
      }
   }
   else
   {
      sprintf(command,"cp %s/dat/GUI.cfg %s/run\n",shomo,shomo);
      system(command);
   }

   // copy second GUI configuration for two-instances run

   if (Multirun > 1)
   {
      sprintf(command,"cp %s/dat/GUI_0?.cfg %s/run\n",shomo,shomo);
      system(command);
   }
   return 1; /* Success */
}

void BuildScripts(void)
{
    // CheckPlasimNamelist();
    // GenerateNames();
    // WritePlasimNamelist();
    if (!Build(PLASIM)) exit(0);
    WriteRunScript(PLASIM);
}

int main(int argc, char *argv[])
{
   BuildScripts();
   return 0;
}
