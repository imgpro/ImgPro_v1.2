//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <CheckLst.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include "CSPIN.h"
#include <FileCtrl.hpp>
#include <Outline.hpp>
#include <Buttons.hpp>
#include <CustomizeDlg.hpp>
#include <ExtDlgs.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TButton *ButtonAdd;
        TListView *ListView1;
        TOpenDialog *OpenDialog1;
        TGroupBox *GroupBox1;
        TEdit *EditImgPath;
        TButton *ButtonBrowseImgFile;
        TOpenDialog *OpenDialog2;
        TButton *ButtonReplace;
        TButton *ButtonDelete;
        TButton *ButtonAbout;
        TButton *ButtonExtract;
        TProgressBar *ProgressBar1;
        void __fastcall ButtonAddClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall ButtonBrowseImgFileClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ButtonReplaceClick(TObject *Sender);
        void __fastcall ButtonDeleteClick(TObject *Sender);
        void __fastcall ButtonAboutClick(TObject *Sender);
        void __fastcall ListView1ColumnClick(TObject *Sender,
          TListColumn *Column);
        void __fastcall ListView1Compare(TObject *Sender, TListItem *Item1,
          TListItem *Item2, int Data, int &Compare);
        void __fastcall ButtonExtractClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
        void __fastcall WmDropFiles(TWMDropFiles& Message);
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        BEGIN_MESSAGE_MAP
		MESSAGE_HANDLER(WM_DROPFILES,TWMDropFiles,WmDropFiles)
	END_MESSAGE_MAP(TForm);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
extern char *CmdStr;
//---------------------------------------------------------------------------
#endif
