//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma resource "*.dfm"
TForm1 *Form1;

//---------------------------------------------------------------------------

const int N_COL_PATH = 2;
const char PRG_NAME[] = "IMG Pro";
const char INI_FILE[] = "imgpro.ini";
//const char PRG_NAME_V[] = "IMG Pro v1.1 beta05";
const char DISK_FULL_MSG[] = "На диске не хватает места";

struct TImgItem{
   int offset;
   int size;
   AnsiString name;
};

struct TImgFile{
   DWORD ver2;
   int count;
   TImgItem *items;
   int *sort;
   int endseek;
   int itmalloc;
};

int sortby, oldsortby;
AnsiString extdir;
char *CmdStr;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

MsgCNOFW(const char *fname, DWORD ErrorCode)
{
    char *strinf;
    strinf = new char[1152];
    wsprintf(strinf,"%s%s%s%d","Не удаётся открыть файл: \n\n",fname,"\n\nI/O error: ",ErrorCode);
    MessageBox(0,strinf,PRG_NAME,MB_ICONWARNING|MB_TASKMODAL);
    return 0;
}
MsgCNOFE(const char *fname, DWORD ErrorCode)
{
    char *strinf;
    strinf = new char[1152];
    wsprintf(strinf,"%s%s%s%d","Не удаётся открыть файл: \n\n",fname,"\n\nI/O error: ",ErrorCode);
    MessageBox(0,strinf,PRG_NAME,MB_ICONERROR|MB_TASKMODAL);
    return 0;
}

bool IsGtaFile(AnsiString FileName)
{
    AnsiString s;
    s = ExtractFileExt(FileName).LowerCase();
    if(s == ".dff")return true;
    if(s == ".txd")return true;
    if(s == ".col")return true;
    if(s == ".ipl")return true;
    if(s == ".ifp")return true;
    if(s == ".scm")return true;
    if(s == ".cut")return true;
    if(s == ".dat")return true;
    if(s == ".rrr")return true;
    if(s == ".anm")return true;
    return false;
}

bool AddToList(AnsiString FileName)
{
    TListItem *itm;
    char *fname;
    fname = new char[MAX_PATH+1];
    char *strinf;
    strinf = new char[1024];
    int c,n;
    _WIN32_FILE_ATTRIBUTE_DATA finf;
    AnsiString s;

    for(c=0; c<FileName.Length() && c<MAX_PATH; c++)fname[c]=FileName[c+1];
    fname[c]=0;

    if(!IsGtaFile(FileName)){
        wsprintf(strinf,"%s%s",fname,"\n\nне является файлом GTA\nдобавить его в список?");
        if(MessageBox(0,strinf,PRG_NAME,MB_TASKMODAL|MB_YESNO|MB_ICONQUESTION) == IDNO)return false;
    }

    itm = Form1->ListView1->Items->Add();
    itm->Caption = ExtractFileName(FileName);

    GetFileAttributesEx(fname,GetFileExInfoStandard,&finf);
    n = finf.nFileSizeLow;
    itm->ImageIndex = n;
    if(finf.nFileSizeLow<1024)s = AnsiString(n);
    else{
        n = n/1024;
        if((finf.nFileSizeLow % 1024 != 0)/*&&(finf.nFileSizeLow % 1024 >= 512)*/)n++;
        s = AnsiString(n);
        s = s+" kb";
    }
    itm->SubItems->Add(s);

    s = ExtractFileExt(FileName);
    for(c=1; c<s.Length(); c++) s[c] = s[c+1];
    s.SetLength(c-1);
    itm->SubItems->Add(s.UpperCase());

    itm->SubItems->Add(ExtractFilePath(FileName));
    return true;
}

void __fastcall TForm1::WmDropFiles(TWMDropFiles& Message)
{
	HDROP drop_handle=(HDROP)Message.Drop;
	char fName[MAX_PATH];
	int filenum=DragQueryFile(drop_handle,-1,NULL,NULL);
	for (int i=0;i<filenum;i++)
	{
		DragQueryFile(drop_handle,i,fName,MAX_PATH);
		AddToList(fName);
	};
	DragFinish(drop_handle);
        if(ListView1->Items->Count > 0){
            ButtonExtract->Enabled = true;
            ButtonReplace->Enabled = true;
        };
}

void __fastcall TForm1::ButtonAddClick(TObject *Sender)
{
   int i;
   if (OpenDialog1->Execute())
   {
        for(i=0; i<OpenDialog1->Files->Count; i++)
        {
            AddToList(OpenDialog1->Files->Strings[i]);
        }
        if(ListView1->Items->Count > 0){
            ButtonExtract->Enabled = true;
            ButtonReplace->Enabled = true;
        }
   }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
        TStringList *list1;
        list1 = new TStringList;
        list1->Add(EditImgPath->Text);
        if(OpenDialog1->FileName == "")
           list1->Add(OpenDialog1->InitialDir);
           else list1->Add(ExtractFilePath(OpenDialog1->FileName));
        list1->Add(extdir);
        list1->SaveToFile(ExtractFilePath(Application->ExeName)+INI_FILE);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonBrowseImgFileClick(TObject *Sender)
{
        if(OpenDialog2->Execute()){
                EditImgPath->Text = OpenDialog2->FileName;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormShow(TObject *Sender)
{
        TStringList *list1;
        int i,n;
        char s[MAX_PATH];
        AnsiString as;
        if(FileExists(ExtractFilePath(Application->ExeName)+INI_FILE)){
            list1 = new TStringList;
            list1->LoadFromFile(ExtractFilePath(Application->ExeName)+INI_FILE);
            as = list1->Strings[0];
            OpenDialog2->InitialDir = ExtractFilePath(list1->Strings[0]);
            OpenDialog1->InitialDir = list1->Strings[1];
            extdir = list1->Strings[2];
        }
        if(CmdStr[0] != 0){
            n = -1;
            for(i=0; CmdStr[i]!=0 && i <= MAX_PATH; i++)
                if(CmdStr[i] == '"'){n=i; break;}
            if(~n){
                n++;
                for(i=0; CmdStr[i]!=0 && i<=MAX_PATH && CmdStr[n+i]!='"'; i++)
                    s[i] = CmdStr[n+i];
                s[i] = 0;
                if(ExtractFileExt(s).UpperCase() == ".IMG")as = s;
            }
        }
        EditImgPath->Text = as;
        sortby = 0;
        oldsortby =0;
}
//---------------------------------------------------------------------------

void ImgSort(TImgFile *ImgFile,int h,int e)
{
    int n,n1,t;
    if(e-h < 1)return;
    n = h;
    n1 = e;
    while(true){
        while(ImgFile->items[ImgFile->sort[n]].offset < ImgFile->items[ImgFile->sort[e]].offset)e--;
        while(h != e){
            h++;
            if(ImgFile->items[ImgFile->sort[n]].offset
                        < ImgFile->items[ImgFile->sort[h]].offset)break;
        }
        if(h == e)break;
        t = ImgFile->sort[h];
        ImgFile->sort[h] = ImgFile->sort[e];
        ImgFile->sort[e] = t;
    }
    t = ImgFile->sort[n];
    ImgFile->sort[n] = ImgFile->sort[e];
    ImgFile->sort[e] = t;
    ImgSort(ImgFile,n,e-1);
    ImgSort(ImgFile,h+1,n1);
}

PutOnThePlace(TImgFile *ImgFile, int n)
{
    int t;
    if(ImgFile->count <= 1)return 0;
    if(n==0)
        if(ImgFile->items[ImgFile->sort[n]].offset > ImgFile->items[ImgFile->sort[n+1]].offset){
            t = ImgFile->sort[n];
            ImgFile->sort[n] = ImgFile->sort[n+1];
            ImgFile->sort[n+1] = t;
            n++;
        }else return 0;
    if(n == ImgFile->count-1)
        if(ImgFile->items[ImgFile->sort[n]].offset < ImgFile->items[ImgFile->sort[n-1]].offset){
            t = ImgFile->sort[n];
            ImgFile->sort[n] = ImgFile->sort[n-1];
            ImgFile->sort[n-1] = t;
            n--;
        }else return 0;

    if((n != 0)&&(n != ImgFile->count-1))
        if(ImgFile->items[ImgFile->sort[n]].offset > ImgFile->items[ImgFile->sort[n+1]].offset)
            for(n; n < ImgFile->count-1; n++){
                if(ImgFile->items[ImgFile->sort[n]].offset > ImgFile->items[ImgFile->sort[n+1]].offset){
                    t = ImgFile->sort[n];
                    ImgFile->sort[n] = ImgFile->sort[n+1];
                    ImgFile->sort[n+1] = t;
                }else return 0;
            }
        else if(ImgFile->items[ImgFile->sort[n]].offset < ImgFile->items[ImgFile->sort[n-1]].offset)
            for(n; n > 0; n--){
                if(ImgFile->items[ImgFile->sort[n]].offset < ImgFile->items[ImgFile->sort[n-1]].offset){
                    t = ImgFile->sort[n];
                    ImgFile->sort[n] = ImgFile->sort[n-1];
                    ImgFile->sort[n-1] = t;
                }else return 0;
            }
    return 0;
}

void IncItmAlloc(TImgFile *ImgFile)
{
    int j;
    TImgItem *itms;
    int *sortitms;

    itms = new TImgItem[ImgFile->count];
    sortitms = new int[ImgFile->count];
    for(j=0; j<ImgFile->count; j++){
        itms[j] = ImgFile->items[j];
        sortitms[j] = ImgFile->sort[j];
    }
    delete [] ImgFile->items;
    delete [] ImgFile->sort;
    ImgFile->itmalloc += 1024;
    ImgFile->items = new TImgItem[ImgFile->itmalloc];
    ImgFile->sort = new int[ImgFile->itmalloc];
    for(j=0; j<ImgFile->count; j++){
        ImgFile->items[j] = itms[j];
        ImgFile->sort[j] = sortitms[j];
    }
    delete [] itms;
    delete [] sortitms;
}

void DelViewItem(AnsiString s)
{
    int n;
    TListItem *itm;
    n = 0;
    while(true){
        itm = Form1->ListView1->FindCaption(n,ExtractFileName(s),true,true,true);
        if(itm == NULL)return;
        if((itm->Caption == ExtractFileName(s))
                &&(itm->SubItems->Strings[N_COL_PATH] == ExtractFilePath(s))){
            Form1->ListView1->Items->Delete(itm->Index);
            return;
            }
        n = itm->Index +1;
    }
}

FindImgSort(TImgFile *ImgFile, AnsiString s)
{
    int i;
    for(i=0; i<ImgFile->count; i++)
        if(s.LowerCase() == ImgFile->items[ImgFile->sort[i]].name.LowerCase())
            return i;
    return -1;
}

FindPlace(TImgFile *ImgFile, int bs)
{
    int n,j,offset;
    n = -1;
    for(j=0; j<ImgFile->count-1; j++)
        if(ImgFile->items[ImgFile->sort[j+1]].offset
            -ImgFile->items[ImgFile->sort[j]].offset
                -ImgFile->items[ImgFile->sort[j]].size >= bs)
                    { n = j; break; }
    if(~n)offset = ImgFile->items[ImgFile->sort[n]].offset+ImgFile->items[ImgFile->sort[n]].size;
    else offset = ImgFile->endseek;
    return offset;
}

WriteImgFromFile(HANDLE himgfile, HANDLE hf, DWORD fsize, DWORD offset)
{
    const int bufsize=128;
    DWORD dwcount,i,t;
    char *buffer;
    buffer = new char[bufsize*2048];
    SetFilePointer(himgfile,offset*2048,NULL,FILE_BEGIN);
    for(i=0; i<fsize/(bufsize*2048); i++){
        ReadFile(hf,buffer,bufsize*2048,&dwcount,NULL);
        WriteFile(himgfile,buffer,bufsize*2048,&dwcount,NULL);
    }
    if(fsize%(bufsize*2048)){
        ZeroMemory(buffer,bufsize*2048);
        ReadFile(hf,buffer,fsize%(bufsize*2048),&dwcount,NULL);
        t = fsize%(bufsize*2048)/2048;
        if(fsize%(bufsize*2048)%2048)t++;
        WriteFile(himgfile,buffer,t*2048,&dwcount,NULL);
    }
    return 0;
}

ReplaceFiles(TStrings* list)
{
        TImgFile *ImgFile;
        ImgFile = new TImgFile;
        int added=0, replaced=0;

        int i,j,n,indeximgsort,t,bs,size;
        DWORD offset;
        HANDLE himgfile,hdirfile,hf;
        char *fname;
        DWORD dwcount;
        fname = new char[1024];
        char *strinf;
        strinf = new char[1152];
        char itmname[25];
        AnsiString s;
        char *buffer1;

        for(i=0; (i<Form1->EditImgPath->Text.Length())&&(i<1023); i++)
                fname[i] = Form1->EditImgPath->Text[i+1];
        fname[i]=0;
        if(!FileExists(Form1->EditImgPath->Text)){
            wsprintf(strinf,"%s%s","IMG файл по указанному пути не найден: \n\n",fname);
            MessageBox(0,strinf,PRG_NAME,MB_ICONERROR|MB_TASKMODAL);
            return 0;
        }
        himgfile = CreateFile(fname, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        t = (DWORD)himgfile;
        if(t == -1){
            MsgCNOFE(fname, GetLastError());
            return 0;
        }
        ImgFile->ver2 = 0;
        ReadFile(himgfile, &ImgFile->ver2, 4, &dwcount, NULL);
        if(ImgFile->ver2 == 0x32524556){
            //MessageBox(0,"ver2","",MB_TASKMODAL);
            ImgFile->count = 0;
            ReadFile(himgfile, &ImgFile->count, 4, &dwcount, NULL);
            ImgFile->itmalloc = ImgFile->count+256;
            ImgFile->items = new TImgItem[ImgFile->itmalloc];
            ImgFile->sort = new int[ImgFile->itmalloc];
            ZeroMemory(itmname,sizeof(itmname));
            for(i=0; i<ImgFile->count; i++){
                ReadFile(himgfile, &ImgFile->items[i].offset, 4, &dwcount, NULL);
                ReadFile(himgfile, &ImgFile->items[i].size, 4, &dwcount, NULL);
                ReadFile(himgfile, &itmname, 24, &dwcount, NULL);
                ImgFile->items[i].name = itmname;
            }
            for(i=0; i<ImgFile->count; i++)ImgFile->sort[i]=i;
            for(i=0; i<ImgFile->count; i++){
                t = ImgFile->sort[i];
                n = rand()%(ImgFile->count);
                ImgFile->sort[i] = ImgFile->sort[n];
                ImgFile->sort[n] = t;
            }
            ImgSort(ImgFile,0,ImgFile->count-1);
            if(ImgFile->count > 0)
                ImgFile->endseek = ImgFile->items[ImgFile->sort[ImgFile->count-1]].offset
                                  +ImgFile->items[ImgFile->sort[ImgFile->count-1]].size;
            else ImgFile->endseek = 1;
            for(i=0; i<list->Count; i++){
                Form1->ProgressBar1->Position = i;

                if(ImgFile->itmalloc == ImgFile->count)IncItmAlloc(ImgFile);

                s = list->Strings[i];
                for(j=0; (j<s.Length())&&(j<1023); j++) fname[j] = s[j+1];
                fname[j] = 0;
                hf = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
                t = (DWORD)hf;
                if(t == -1){
                    MsgCNOFW(fname, GetLastError());
                    continue;
                }
                size = GetFileSize(hf,0);
                bs = size/2048;
                if(size%2048)bs++;

                indeximgsort = FindImgSort(ImgFile,ExtractFileName(s));

                if(indeximgsort != -1){
                    n = -1;
                    offset = FindPlace(ImgFile,bs);
                    WriteImgFromFile(himgfile,hf,size,offset);
                    if(!GetLastError()){
                        ImgFile->items[ImgFile->sort[indeximgsort]].offset = offset;
                        ImgFile->items[ImgFile->sort[indeximgsort]].size = bs;
                        SetFilePointer(himgfile,8+ImgFile->sort[indeximgsort]*32,NULL,FILE_BEGIN);
                        WriteFile(himgfile,&offset,4,&dwcount,NULL);
                        WriteFile(himgfile,&bs,4,&dwcount,NULL);
                        PutOnThePlace(ImgFile,indeximgsort);
                        ImgFile->endseek = ImgFile->items[ImgFile->sort[ImgFile->count-1]].offset
                                          +ImgFile->items[ImgFile->sort[ImgFile->count-1]].size;
                        replaced++;
                    }else{
                        if(GetLastError()==112)MessageBox(0,DISK_FULL_MSG,PRG_NAME,MB_ICONERROR|MB_TASKMODAL);
                        else MessageBox(0,"I/O error",PRG_NAME,MB_ICONERROR|MB_TASKMODAL);
                        CloseHandle(hf);
                        CloseHandle(himgfile);
                        return 0;
                    }
                }else{
                   if(ImgFile->count > 0)
                    if(40+ImgFile->count*32 > ImgFile->items[ImgFile->sort[0]].offset*2048){
                        offset = FindPlace(ImgFile,ImgFile->items[ImgFile->sort[0]].size);

                        buffer1 = new char[ImgFile->items[ImgFile->sort[0]].size*2048];
                        SetFilePointer(himgfile,ImgFile->items[ImgFile->sort[0]].offset*2048,NULL,FILE_BEGIN);
                        ReadFile(himgfile,buffer1,ImgFile->items[ImgFile->sort[0]].size*2048,&dwcount,NULL);
                        SetFilePointer(himgfile,offset*2048,NULL,FILE_BEGIN);
                        WriteFile(himgfile,buffer1,ImgFile->items[ImgFile->sort[0]].size*2048,&dwcount,NULL);

                        if(!GetLastError()){
                            t = ImgFile->items[ImgFile->sort[0]].offset;
                            ImgFile->items[ImgFile->sort[0]].offset = offset;
                            SetFilePointer(himgfile,8+ImgFile->sort[0]*32,NULL,FILE_BEGIN);
                            WriteFile(himgfile,&offset,4,&dwcount,NULL);
                            ZeroMemory(buffer1,ImgFile->items[ImgFile->sort[0]].size*2048);
                            SetFilePointer(himgfile,t,NULL,FILE_BEGIN);
                            WriteFile(himgfile,buffer1,ImgFile->items[ImgFile->sort[0]].size*2048,&dwcount,NULL);
                            delete [] buffer1;
                            PutOnThePlace(ImgFile,ImgFile->sort[0]);
                            ImgFile->endseek = ImgFile->items[ImgFile->sort[ImgFile->count-1]].offset
                                              +ImgFile->items[ImgFile->sort[ImgFile->count-1]].size;
                        }else{
                            if(GetLastError()==112)MessageBox(0,DISK_FULL_MSG,PRG_NAME,MB_ICONERROR|MB_TASKMODAL);
                            else MessageBox(0,"I/O error",PRG_NAME,MB_ICONERROR|MB_TASKMODAL);
                            CloseHandle(hf);
                            CloseHandle(himgfile);
                            return 0;
                        }
                    }
                    offset = FindPlace(ImgFile,bs);
                    WriteImgFromFile(himgfile,hf,size,offset);
                    if(!GetLastError()){
                        t = ImgFile->count;
                        ImgFile->items[t].offset = offset;
                        ImgFile->items[t].size = bs;
                        ImgFile->items[t].name = ExtractFileName(list->Strings[i]);
                        ImgFile->sort[t] = t;
                        SetFilePointer(himgfile,8+t*32,NULL,FILE_BEGIN);
                        WriteFile(himgfile,&offset,4,&dwcount,NULL);
                        WriteFile(himgfile,&bs,4,&dwcount,NULL);
                        s = ImgFile->items[t].name;
                        ZeroMemory(&itmname,25);
                        for(j=0; j<s.Length() && j<24; j++)itmname[j] = s[j+1];
                        WriteFile(himgfile,&itmname,24,&dwcount,NULL);
                        ImgFile->count++;
                        PutOnThePlace(ImgFile,t);
                        ImgFile->endseek = ImgFile->items[ImgFile->sort[ImgFile->count-1]].offset
                                          +ImgFile->items[ImgFile->sort[ImgFile->count-1]].size;
                        SetFilePointer(himgfile,4,NULL,FILE_BEGIN);
                        WriteFile(himgfile,&ImgFile->count,4,&dwcount,NULL);
                        added++;
                    }else{
                        if(GetLastError()==112)MessageBox(0,DISK_FULL_MSG,PRG_NAME,MB_ICONERROR|MB_TASKMODAL);
                        else MessageBox(0,"I/O error","IMG Pro",MB_ICONERROR|MB_TASKMODAL);
                        CloseHandle(hf);
                        CloseHandle(himgfile);
                        return 0;
                    }
                }
                DelViewItem(list->Strings[i]);
                CloseHandle(hf);
            }
            Form1->ProgressBar1->Position = i;
        }else{
            if(!FileExists(ChangeFileExt(Form1->EditImgPath->Text,".dir"))){
                MessageBox(0,"Unable to find DIR file!","Error",MB_ICONSTOP|MB_TASKMODAL);
                CloseHandle(himgfile);
                return 0;
            }
            s = ChangeFileExt(Form1->EditImgPath->Text,".dir");
            for(i=0; i<s.Length(); i++) fname[i] = s[i+1];
            fname[i] = 0;
            hdirfile = CreateFile(fname, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
            t = (DWORD)hdirfile;
            if(t == -1){
                MsgCNOFE(fname, GetLastError());
                return 0;
            }
            
            size = GetFileSize(hdirfile, NULL);
            ImgFile->count = size/32;
            ImgFile->itmalloc = ImgFile->count +256;
            ImgFile->items = new TImgItem[ImgFile->itmalloc];
            ImgFile->sort = new int[ImgFile->itmalloc];
            ZeroMemory(itmname,sizeof(itmname));
            for(i=0; i<ImgFile->count; i++){
                ReadFile(hdirfile, &ImgFile->items[i].offset, 4, &dwcount, NULL);
                ReadFile(hdirfile, &ImgFile->items[i].size, 4, &dwcount, NULL);
                ReadFile(hdirfile, &itmname, 24, &dwcount, NULL);
                ImgFile->items[i].name = itmname;
            }
            for(i=0; i<ImgFile->count; i++)ImgFile->sort[i]=i;
            for(i=0; i<ImgFile->count; i++){
                t = ImgFile->sort[i];
                n = rand()%(ImgFile->count);
                ImgFile->sort[i] = ImgFile->sort[n];
                ImgFile->sort[n] = t;
            }
            ImgSort(ImgFile,0,ImgFile->count-1);
            if(ImgFile->count > 0)
                ImgFile->endseek = ImgFile->items[ImgFile->sort[ImgFile->count-1]].offset
                                  +ImgFile->items[ImgFile->sort[ImgFile->count-1]].size;
            else ImgFile->endseek = 0;
            for(i=0; i<list->Count; i++){
                Form1->ProgressBar1->Position = i;

                if(ImgFile->itmalloc == ImgFile->count)IncItmAlloc(ImgFile);

                s = list->Strings[i];
                for(j=0; j<s.Length(); j++) fname[j] = s[j+1];
                fname[j] = 0;
                hf = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
                t = (DWORD)hf;
                if(t == -1){
                    MsgCNOFW(fname, GetLastError());
                    continue;
                }
                size = GetFileSize(hf,0);
                bs = size/2048;
                if(size%2048)bs++;

                if(ImgFile->count > 0)
                    if(ImgFile->items[ImgFile->sort[0]].offset >= bs)offset = 0;
                    else offset = FindPlace(ImgFile,bs);
                else offset = 0;
                WriteImgFromFile(himgfile,hf,size,offset);
                if(!GetLastError()){
                    indeximgsort = FindImgSort(ImgFile,ExtractFileName(s));
                    if(indeximgsort != -1){
                        ImgFile->items[ImgFile->sort[indeximgsort]].offset = offset;
                        ImgFile->items[ImgFile->sort[indeximgsort]].size = bs;
                        SetFilePointer(hdirfile,ImgFile->sort[indeximgsort]*32,NULL,FILE_BEGIN);
                        WriteFile(hdirfile,&offset,4,&dwcount,NULL);
                        WriteFile(hdirfile,&bs,4,&dwcount,NULL);
                        replaced++;
                    }else{
                        t = ImgFile->count;
                        ImgFile->count++;
                        ImgFile->sort[t] = t;
                        ImgFile->items[t].offset = offset;
                        ImgFile->items[t].size = bs;
                        s = ExtractFileName(list->Strings[i]);
                        ImgFile->items[t].name = s;
                        SetFilePointer(hdirfile,t*32,NULL,FILE_BEGIN);
                        WriteFile(hdirfile,&offset,4,&dwcount,NULL);
                        WriteFile(hdirfile,&bs,4,&dwcount,NULL);
                        ZeroMemory(itmname,sizeof(itmname));
                        for(j=0; j<s.Length() && j<24; j++)itmname[j] = s[j+1];
                        WriteFile(hdirfile,&itmname,24,&dwcount,NULL);
                        indeximgsort = t;
                        added++;
                    }
                    PutOnThePlace(ImgFile,indeximgsort);
                    ImgFile->endseek = ImgFile->items[ImgFile->sort[ImgFile->count-1]].offset
                                      +ImgFile->items[ImgFile->sort[ImgFile->count-1]].size;
                }else{
                    if(GetLastError()==112)MessageBox(0,DISK_FULL_MSG,"IMG Pro",MB_ICONERROR|MB_TASKMODAL);
                    else MessageBox(0,"I/O error","IMG Pro",MB_ICONERROR|MB_TASKMODAL);
                    CloseHandle(hf);
                    CloseHandle(himgfile);
                    CloseHandle(hdirfile);
                    return 0;
                }
                DelViewItem(list->Strings[i]);
                CloseHandle(hf);
            }
            Form1->ProgressBar1->Position = i;
            CloseHandle(hdirfile);
        }
        CloseHandle(himgfile);
        wsprintf(strinf,"%s%d%s%d","Завершено\n\nЗаменено: ",replaced,"\nДобавлено: ",added);
        MessageBox(0, strinf, "IMG Pro", MB_ICONASTERISK|MB_TASKMODAL);
        return 0;
}

void __fastcall TForm1::ButtonReplaceClick(TObject *Sender)
{
        //VER2 = 0x32524556

        int i;
        /*int c,j,f;
        TListItem *itm;
        int allbsize,incsize;*/

        /*HANDLE hf;
        DWORD size;
        AnsiString s;
        char ndrive; */

        TStringList *list;
        list = new TStringList;

        if(MessageBox(0, "Заменить?", PRG_NAME, MB_YESNO|MB_ICONQUESTION|MB_TASKMODAL) & 1)
                return;

        /*allbsize = 0;
        for(i=0; i<ListView1->Items->Count; i++){ //посчитать общий размер заменяемых файлов в блоках по 2048;
                itm = ListView1->Items->Item[i];
                allbsize += itm->ImageIndex/2048;
                if(itm->ImageIndex%2048 != 0)allbsize++;
        }

        incsize = ListView1->Items->Count*32 + allbsize*2048 +65536;
        ProcessPath(EditImgPath->Text,ndrive,s,s);
        ndrive -= 64;
        if(DiskFree(ndrive)<incsize){
                MessageBox(0, "Недостаточно места на диске", "Error", MB_OK|MB_ICONERROR);
                return;
        } */
        Form1->ButtonExtract->Enabled = false;
        Form1->ButtonReplace->Enabled = false;
        Form1->ButtonAdd->Enabled = false;
        Form1->ButtonDelete->Enabled = false;
        ProgressBar1->Max = ListView1->Items->Count;
        ProgressBar1->Position = 0;

        for(i=0; i<ListView1->Items->Count; i++)
            list->Add(ListView1->Items->Item[i]->SubItems->Strings[N_COL_PATH]+ListView1->Items->Item[i]->Caption);
        ReplaceFiles(list);

        if(ListView1->Items->Count > 0){
            Form1->ButtonExtract->Enabled = true;
            Form1->ButtonReplace->Enabled = true;
        }
        Form1->ButtonAdd->Enabled = true;
        Form1->ButtonDelete->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonDeleteClick(TObject *Sender)
{
        ListView1->DeleteSelected();
        if(ListView1->Items->Count <= 0){
            Form1->ButtonExtract->Enabled = false;
            Form1->ButtonReplace->Enabled = false;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonAboutClick(TObject *Sender)
{
    //MessageBox(0,"Эту прогу сделал LOD    E-mail: imgpro@rambler.ru","About",0);
    //imgpro@rambler.ru
    AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ListView1ColumnClick(TObject *Sender,
      TListColumn *Column)
{
        oldsortby = sortby;
        sortby = Column->Index;
        ListView1->AlphaSort();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ListView1Compare(TObject *Sender, TListItem *Item1,
      TListItem *Item2, int Data, int &Compare)
{
        AnsiString s1,s2,s3,s4;
        int c;

        if(sortby == 0){
                s1 = Item1->Caption;
                for(c=1; c<=s1.Length(); c++)
                        if((s1[c]>=65)&&(s1[c]<=90))s1[c] = s1[c]+32;
                s2 = Item2->Caption;
                for(c=1; c<=s2.Length(); c++)
                        if((s2[c]>=65)&&(s2[c]<=90))s2[c] = s2[c]+32;
        }else{
                s1 = Item1->SubItems->Strings[sortby-1];
                s2 = Item2->SubItems->Strings[sortby-1];
        }
        if(oldsortby == 0){
                s3 = Item1->Caption;
                for(c=1; c<=s3.Length(); c++)
                        if((s3[c]>=65)&&(s3[c]<=90))s3[c] = s3[c]+32;
                s4 = Item2->Caption;
                for(c=1; c<=s4.Length(); c++)
                        if((s4[c]>=65)&&(s4[c]<=90))s4[c] = s4[c]+32;
        }else{
                s3 = Item1->SubItems->Strings[oldsortby-1];
                s4 = Item2->SubItems->Strings[oldsortby-1];
        }

        if(sortby==1){
                if(Item1->ImageIndex < Item2->ImageIndex)Compare=-1;
                else if(Item1->ImageIndex > Item2->ImageIndex)Compare=+1;
                else if(oldsortby==sortby)Compare=0;
                else if(s3<s4)Compare = -1;
                else if(s3>s4)Compare = +1;
                else Compare = 0;
        }else{
                if(s1<s2)Compare = -1;
                else if(s1>s2)Compare = +1;
                else if(sortby==oldsortby)Compare = 0;
                else if(oldsortby==1)
                        if(Item1->ImageIndex < Item2->ImageIndex)Compare=-1;
                        else if(Item1->ImageIndex > Item2->ImageIndex)Compare=+1;
                        else Compare = 0;
                     else if(s3<s4)Compare = -1;
                     else if(s3>s4)Compare = +1;
                     else Compare = 0;
        }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonExtractClick(TObject *Sender)
{
    wchar_t wstr[2];
    AnsiString dlgcapt;
    int i,n,j;

    TImgFile *ImgFile;
    ImgFile = new TImgFile;
    int extracted=0;

    int t,bs,size,offset;
    HANDLE himgfile,hdirfile,hf;
    _WIN32_FILE_ATTRIBUTE_DATA finf;
    SYSTEMTIME st;
    char *fname;
    fname = new char[1024];
    DWORD dwcount;
    char *strinf;
    strinf = new char[1280];
    char *date;
    date = new char[64];
    char itmname[25];
    AnsiString s;
    const int bufsize=128;
    char *buffer;
    buffer = new char[bufsize*2048];

    dlgcapt = "Choose the directory you want extract to...";
    StringToWideChar("",wstr,2);
    if(SelectDirectory(dlgcapt,wstr,extdir)){
        ProgressBar1->Max = ListView1->Items->Count;
        ProgressBar1->Position = 0;
        for(i=0; (i<Form1->EditImgPath->Text.Length())&&(i<1023); i++)
                fname[i] = Form1->EditImgPath->Text[i+1];
        fname[i]=0;
        if(!FileExists(Form1->EditImgPath->Text)){
            wsprintf(strinf,"%s%s","IMG файл по указанному пути не найден: \n\n",fname);
            MessageBox(0,strinf,PRG_NAME,MB_ICONERROR|MB_TASKMODAL);
            return;
        }
        himgfile = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        t = (DWORD)himgfile;
        if(t == -1){
            MsgCNOFE(fname, GetLastError());
            return;
        }
        ImgFile->ver2 = 0;
        ReadFile(himgfile, &ImgFile->ver2, 4, &dwcount, NULL);
        if(ImgFile->ver2 == 0x32524556){
            ImgFile->count = 0;
            ReadFile(himgfile, &ImgFile->count, 4, &dwcount, NULL);
            ImgFile->itmalloc = ImgFile->count+256;
            ImgFile->items = new TImgItem[ImgFile->itmalloc];
            ZeroMemory(itmname,sizeof(itmname));
            for(i=0; i<ImgFile->count; i++){
                ReadFile(himgfile, &ImgFile->items[i].offset, 4, &dwcount, NULL);
                ReadFile(himgfile, &ImgFile->items[i].size, 4, &dwcount, NULL);
                ReadFile(himgfile, &itmname, 24, &dwcount, NULL);
                ImgFile->items[i].name = itmname;
            }
        }else{
            if(!FileExists(ChangeFileExt(Form1->EditImgPath->Text,".dir"))){
                MessageBox(0,"Unable to find DIR file!","Error",MB_ICONSTOP|MB_TASKMODAL);
                CloseHandle(himgfile);
                return;
            }
            s = ChangeFileExt(Form1->EditImgPath->Text,".dir");
            for(i=0; i<s.Length(); i++) fname[i] = s[i+1];
            fname[i] = 0;
            hdirfile = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
            t = (DWORD)hdirfile;
            if(t == -1){
                MsgCNOFE(fname, GetLastError());
                return;
            }
            size = GetFileSize(hdirfile, NULL);
            ImgFile->count = size/32;
            ImgFile->itmalloc = ImgFile->count +256;
            ImgFile->items = new TImgItem[ImgFile->itmalloc];
            ZeroMemory(itmname,sizeof(itmname));
            for(i=0; i<ImgFile->count; i++){
                ReadFile(hdirfile, &ImgFile->items[i].offset, 4, &dwcount, NULL);
                ReadFile(hdirfile, &ImgFile->items[i].size, 4, &dwcount, NULL);
                ReadFile(hdirfile, &itmname, 24, &dwcount, NULL);
                ImgFile->items[i].name = itmname;
            }
        }
        Form1->ButtonExtract->Enabled = false;
        Form1->ButtonReplace->Enabled = false;
        Form1->ButtonAdd->Enabled = false;
        Form1->ButtonDelete->Enabled = false;
        for(i=0; i<ListView1->Items->Count; i++){
            ProgressBar1->Position = i;
            n = -1;
            for(j=0; j<ImgFile->count; j++)
                if(ImgFile->items[j].name.LowerCase() == ListView1->Items->Item[i]->Caption.LowerCase()){
                    n = j;
                    break;
                }
            if(n == -1)continue;
            s = extdir + "\\" + ImgFile->items[n].name;
            for(j=0; (j<s.Length())&&(j<1023); j++) fname[j] = s[j+1];
            fname[j] = 0;
            if(FileExists(s)){
                ZeroMemory(&finf,sizeof finf);
                GetFileAttributesEx(fname,GetFileExInfoStandard,&finf);
                FileTimeToSystemTime(&finf.ftLastWriteTime,&st);
                wsprintf(date,"%d.%d.%d  %d:%d:%d",st.wDay,st.wMonth,st.wYear,st.wHour,st.wMinute,st.wSecond);
                wsprintf(strinf,"%s   уже существует.\n\nЗаменить имеющийся файл\nразмер: %db\nдата изменения: %s\n\nИзвлекаемым файлом?\nразмер: %db",
                        fname,finf.nFileSizeLow,date,ImgFile->items[n].size*2048);
                t = MessageBox(NULL,strinf,PRG_NAME,MB_YESNOCANCEL|MB_TASKMODAL|MB_ICONQUESTION);
                if(t == IDCANCEL)break;
                if(t == IDNO)continue;
                if(t == IDYES)
                  if(FileExists(s))
                    hf = CreateFile(fname, GENERIC_WRITE, 0, NULL, TRUNCATE_EXISTING, 0, NULL);
                  else hf = CreateFile(fname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
            }else
                hf = CreateFile(fname, GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL);
            t = (DWORD)hf;
            if(t == -1){
              j = GetLastError();
              if(j == 112){
                wsprintf(strinf,"%s%s%s%s","Не удаётся создать файл: \n\n",fname,"\n\n",DISK_FULL_MSG);
                MessageBox(NULL,DISK_FULL_MSG,PRG_NAME,0);
              }else{
                wsprintf(strinf,"%s%s%s%d","Не удаётся создать файл: \n\n",fname,"\n\nI/O error: ",j);
                MessageBox(0,strinf,PRG_NAME,MB_ICONWARNING|MB_TASKMODAL);
              }
                continue;
            }
            SetFilePointer(himgfile,ImgFile->items[n].offset*2048,NULL,FILE_BEGIN);
            for(j=0; j<ImgFile->items[n].size/bufsize; j++){
                ReadFile(himgfile,buffer,bufsize*2048,&dwcount,NULL);
                WriteFile(hf,buffer,bufsize*2048,&dwcount,NULL);
            }
            if(ImgFile->items[n].size%bufsize != 0){
                ReadFile(himgfile,buffer,ImgFile->items[n].size%bufsize*2048,&dwcount,NULL);
                WriteFile(hf,buffer,ImgFile->items[n].size%bufsize*2048,&dwcount,NULL);
            }
            t = GetLastError();
            if(t){
                wsprintf(strinf,"%s%s%d",fname,"  записан с ошибкой ",t);
                MessageBox(NULL,strinf,PRG_NAME,MB_TASKMODAL|MB_ICONWARNING);
            }
            extracted++;
            CloseHandle(hf);
        }
        ProgressBar1->Position = i;
        if(ImgFile->ver2 != 0x32524556)CloseHandle(hdirfile);
        CloseHandle(himgfile);
        wsprintf(strinf,"%s%d","Извлечено: ",extracted);
        MessageBox(0,strinf,PRG_NAME,MB_TASKMODAL|MB_ICONINFORMATION);
        if(ListView1->Items->Count > 0){
            Form1->ButtonExtract->Enabled = true;
            Form1->ButtonReplace->Enabled = true;
        }
        Form1->ButtonAdd->Enabled = true;
        Form1->ButtonDelete->Enabled = true;
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
        DragAcceptFiles(Handle,true);
}
//---------------------------------------------------------------------------

