//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ScktComp.hpp>
#include "Trayicon.h"
#include <Menus.hpp>
#include <ImgList.hpp>
#include <ActnList.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:  // IDE-managed Components
        TServerSocket *ServerSocket1;
        TLabel *Label1;
        TGroupBox *GroupBox1;
        TListView *ListView1;
        TGroupBox *GroupBox2;
        TButton *Button1;
        TButton *Button2;
        TButton *Button3;
        TTrayIcon *TrayIcon1;
        TPopupMenu *PopupMenu1;
        TMenuItem *N1;
        TMenuItem *N2;
        TMenuItem *N3;
        TMenuItem *N4;
        TMenuItem *N5;
        TMenuItem *N6;
        TImageList *ImageList1;
        TActionList *ActionList1;
        TAction *ActionStart;
        TAction *ActionStop;
        TAction *ActionExit;
        TMemo *Memo1;
        void __fastcall ServerSocket1ClientConnect(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ServerSocket1ClientDisconnect(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ServerSocket1ClientError(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
        void __fastcall ServerSocket1ClientRead(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall N1Click(TObject *Sender);
        void __fastcall N6Click(TObject *Sender);
        void __fastcall N3Click(TObject *Sender);
        void __fastcall N4Click(TObject *Sender);
        void __fastcall ActionStartExecute(TObject *Sender);
        void __fastcall ActionStopExecute(TObject *Sender);
        void __fastcall ActionExitExecute(TObject *Sender);
        void __fastcall FormCanResize(TObject *Sender, int &NewWidth,
          int &NewHeight, bool &Resize);
        void __fastcall FormResize(TObject *Sender);
private:  // User declarations
public:
        __fastcall TForm1(TComponent* Owner);
        int __fastcall AddClient(AnsiString Addr, AnsiString name, int id, TCustomWinSocket * socket);
        void __fastcall UpdateUserInformation(int n);
        void __fastcall MakeCompTurn(int index,int x,int y);
        void __fastcall MakeCompNew(int id);
        int __fastcall FindWinner(int id,int x, int y);
        void __fastcall GetCompTurn(int index, int x, int y);
        void __fastcall Log(AnsiString a);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
