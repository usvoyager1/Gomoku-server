//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Trayicon"
#pragma resource "*.dfm"


TForm1 *Form1;
enum UserStatus {STATUS_CONNECTED,STATUS_PLAYING,STATUS_WAITING};
typedef struct
{
  bool used;
  int id;
  int opp_id;
  TCustomWinSocket *Socket;
  AnsiString Name;
  AnsiString Addr;
  UserStatus Status;
} USER;
#define MAX_USERS 15
#define MAX_COMPS 3
enum {MY_TURN,OPP_TURN} Turns[MAX_COMPS];
int movecount[MAX_COMPS];
USER users[MAX_USERS];
typedef struct{
  bool used;
  int opp_id;
  UserStatus Status;
} SUSER;
SUSER susers[5];  // computers
int ClientID=MAX_COMPS-1;   // 0,1,2,3,4 - Computer 1, Computer 2, Computer 3, ...

int ppp[MAX_COMPS][20][20],xxx[MAX_COMPS][20][20],ooo[MAX_COMPS][20][20];
int idir[]={1,1,1,0};
int jdir[]={-1,0,1,1};
int ccc[]={36,64,100,81,112,121,169};

int FindUserBySocket(TCustomWinSocket *s)
{
  for(int i=0;i<MAX_USERS;i++)
    if(users[i].used&&users[i].Socket==s)
      return i;
  return MAX_USERS;
}
int FindUserByID(int id)
{
  for(int i=0;i<MAX_USERS;i++)
    if(users[i].used&&users[i].id==id)
      return i;
  return MAX_USERS;
}
AnsiString StatusToStr(UserStatus st)
{
  switch(st){
    case STATUS_CONNECTED:return "подсоединен";
    case STATUS_PLAYING:return "играет";
    case STATUS_WAITING:return "ожидает";
    default:return "неизвестен";
  }
}
void SendMsg(TCustomWinSocket *s,AnsiString str)
{
  Form1->Log("Sending message: "+str);
  s->SendText(str);
  s->SendBuf("\n",1);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void find_moves(int index,int &a,int &b){
  int i,j,k,m,n,o,r,v,x;
  bool accepted;
  int opp=users[index].opp_id;
  int x_best[21],o_best[21],save[21];
  m=0; o=0;
  for(i=1;i<=20;i++){
    for(j=1;j<=20;j++){
      v=ooo[opp][i-1][j-1];
      if(v>=o){
        if(v>o)m=1; else if(m<20)m++;
        o_best[m-1]=(i<<8)|j;o=v;
      }
    }
  }
  accepted=false;
  do{
    n=0; x=0;
    for(i=1;i<=20;i++){
      for(j=1;j<=20;j++){
        v=xxx[opp][i-1][j-1];
        if(v>=x){
          if(v>x)n=1; else if(n<20)n++;
          x_best[n-1]=(i<<8)|j;x=v;
        }
      }
    }
    if(x==112 && (random(2)+1)==1 )
      xxx[opp][(x_best[0]>>8)-1][(x_best[0]&0xFF)-1]=82;
    else accepted=true;
  }while(!accepted);
  r=0;v=0;
  if( (x>=o || o<200)  && x>0 && (random(2)+1)==1 ){
    if(o<=25){ memcpy(save,x_best,sizeof(save)); r=n;}
    else{
      for(k=1;k<=n;k++){
        o=ooo[opp][(x_best[k-1]>>8)-1][(x_best[k-1]&0xFF)-1];
        if(o>=v){
          if(o>v)r=1; else r++;
          save[r-1]=x_best[k-1]; v=o;
        }
      }
    }
  }else{
    for(k=1;k<=m;k++){
      x=xxx[opp][(o_best[k-1]>>8)-1][(o_best[k-1]&0xFF)-1];
      if(x>=v){
        if(x>v)r=1; else r++;
        save[r-1]=o_best[k-1];v=x;
      }
    }
  }
  r=random(r)+1;
  a=(save[r-1]>>8)-1; b=(save[r-1]&0xFF)-1;
}

int scan4(int index,int p,int i,int j){
  int a,b,d,k,m,n,o,r,s,sgn,t,v,x;
  int vvv[5];
  int ttt[3];
  int opp=users[index].opp_id;
  for(d=1;d<5;d++){
    r=0; s=0; t=0; v=0; sgn=1;
    for(n=1;n<=2;n++){
      sgn=-sgn; m=5;
      for(k=1;k<=4;k++){
        a=i+sgn*k*idir[d-1]; b=j+sgn*k*jdir[d-1]; x=-1;
        if ((a<1) || (a>20) || (b<1) || (b>20) || ( x==(3-p) )){
          if(m==5)t=2; else if( m==3 && o==p )r=1;
          break;
        }else  x=ppp[opp][a-1][b-1];
        if(x==p)v=v+m; else m=(m+1)/2;
        o=x;s++;
      }
    }
    if(s<4)v=0; else v*=v;
    if(v<400)if(t==2)v/=2; else v-=r;
    vvv[d]=v;
  }
  m=1;
  for(n=1;n<=2;n++){
    for(d=1;d<=4;d++)if(vvv[d]>vvv[m])m=d;
    v=vvv[m]; vvv[m]=0;
    if(v>=400)v+=v;
    if(v>=255)v=(v+v)*p;
    ttt[n]=v;
  }
  int ret=ttt[1]+ttt[2];
  for(n=1;n<=7;n++){
    for(m=1;m<=7;m++){
      if ( ttt[1]==ccc[n-1] && ttt[2]==ccc[m-1] ){
        if ( m>3 || n>3 )ret=400*p; else ret=200;
        return ret;
      }
    }
  }
  return ret;
}

void set_values(int index,int i,int j){
  int a,b,sgn,v,w,x,y,z;
  int opp=users[index].opp_id;
  ooo[opp][i][j]=xxx[opp][i][j]=0;
  sgn=1;
  i++;j++;
  for(w=1;w<=4;w++){
    for(x=1;x<=2;x++){
      for(y=1;y<=2;y++){
        sgn=-sgn;
        for(z=1;z<=4;z++){
          a=i+sgn*z*idir[w-1];b=j+sgn*z*jdir[w-1];
          if( a<1 || a>20 || b<1 || b>20 )break;
          if(ppp[opp][a-1][b-1]==0){
            if(x==1)ooo[opp][a-1][b-1]=scan4(index,x,a,b); else xxx[opp][a-1][b-1]=scan4(index,x,a,b);
          }else if(x!=ppp[opp][a-1][b-1])break;
        }
      }
    }
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ServerSocket1ClientConnect(TObject *Sender,
      TCustomWinSocket *Socket)
{

  ClientID++;
  if(AddClient(Socket->RemoteAddress,"unnamed",ClientID,Socket))
  {
    SendMsg(Socket,"Cannot connect! Too many clients connected.");
//    Socket->Close();
    ClientID--;
    return;
  }
  Log("Client connected: ID="+IntToStr(ClientID));
  SendMsg(Socket,AnsiString("CONNECTED=")+ClientID);

  for(int i=0;i<MAX_COMPS;i++){
    if(!susers[i].used){
      SendMsg(Socket,AnsiString("ADD_OPPONENT=")+i+",Computer "+IntToStr(i+1));
    }
  }
  for(int i=0;i<MAX_USERS;i++){
    if(users[i].used){
      if(users[i].id!=ClientID){
        if(users[i].Status==STATUS_CONNECTED || users[i].Status==STATUS_WAITING)
          SendMsg(Socket,AnsiString("ADD_OPPONENT=")+users[i].id+","+users[i].Name);
        SendMsg(users[i].Socket,AnsiString("ADD_OPPONENT=")+ClientID+",unnamed");
      }
    }
  }
}
//---------------------------------------------------------------------------
int __fastcall TForm1::AddClient(AnsiString Addr, AnsiString name, int id, TCustomWinSocket * socket)
{
  TListItem *it;

  Log("Adding new client...");

  int i;
  for(i=0;i<MAX_USERS;i++)
    if(!users[i].used)break;
  if(i==MAX_USERS)
  {
    Log("Cannot connect client! Too many clients connected.");
    return -1;
  }

  it=ListView1->Items->Add();
  it->Caption=Addr;
  it->SubItems->Add(name);
  it->SubItems->Add("connected");
  it->SubItems->Add(id);
  users[i].Addr=Addr;
  users[i].Name=name;
  users[i].Status=STATUS_CONNECTED;
  users[i].Socket=socket;
  users[i].used=true;
  users[i].id=id;
  return 0;
}
void __fastcall TForm1::ServerSocket1ClientDisconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
  int i=FindUserBySocket(Socket);
  if(i==MAX_USERS)return;
  Log("Client disconnected: ID="+IntToStr(users[i].id));
  for(int j=0;j<ListView1->Items->Count;j++)
    if(StrToInt(ListView1->Items->Item[j]->SubItems->Strings[2])==users[i].id){
      ListView1->Items->Delete(j);
      break;
    }
  users[i].used=false;
  if(users[i].Status==STATUS_PLAYING){
    if(users[i].opp_id<MAX_COMPS){
      int o=users[i].opp_id;
      susers[o].Status=STATUS_CONNECTED;
      susers[o].used=false;
      for(int j=0;j<MAX_USERS;j++)
        if(users[j].used && j!=i)
          SendMsg(users[j].Socket,AnsiString("ADD_OPPONENT=")+o+",Computer "+IntToStr(o+1));
    }else{
      int o=FindUserByID(users[i].opp_id);
      SendMsg(users[o].Socket,"PLAY_CANCEL");
      users[o].Status=STATUS_CONNECTED;
      UpdateUserInformation(o);
      for(int j=0;j<MAX_USERS;j++)
        if(users[j].used && j!=o && j!=i)
          SendMsg(users[j].Socket,AnsiString("ADD_OPPONENT=")+users[o].id+","+users[o].Name);
    }
  }

  for(int j=0;j<MAX_USERS;j++)
    if(users[j].used && j!=i)
      SendMsg(users[j].Socket,AnsiString("DEL_OPPONENT=")+users[i].id);

  for(int j=0;j<MAX_USERS;j++)
    if(users[j].used && j!=i && users[j].Status==STATUS_WAITING && users[j].opp_id==users[i].id){
      SendMsg(users[j].Socket,"PLAY_CANCEL");
      users[j].Status=STATUS_CONNECTED;
      UpdateUserInformation(j);
      for(int k=0;k<MAX_USERS;k++)
        if(users[k].used && k!=j && k!=i)
          SendMsg(users[k].Socket,AnsiString("ADD_OPPONENT=")+users[j].id+","+users[j].Name);
    }
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ServerSocket1ClientError(TObject *Sender,
      TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
  switch(ErrorEvent)
  {
    case eeGeneral:
      Log("General network error");
      break;
    case eeConnect:
      Log("Error while connect");
      break;
    case eeDisconnect:
      if(ErrorCode==10053)ServerSocket1ClientDisconnect(ServerSocket1,Socket);
      else if(ErrorCode==10054){
        ServerSocket1ClientDisconnect(Sender,Socket);
      }else
        Log(AnsiString("Error while disconnect:")+ErrorCode);
      break;
    default:
      Log(AnsiString("Unknown (")+ErrorEvent+") error: #"+ErrorCode);
      break;
  }
  ErrorCode=0;
}
//---------------------------------------------------------------------------
AnsiString bufAll[MAX_USERS];
void __fastcall TForm1::ServerSocket1ClientRead(TObject *Sender,
      TCustomWinSocket *Socket)
{
  AnsiString msg;
  int msglen=Socket->ReceiveLength();
  int i=FindUserBySocket(Socket);
  if(i==MAX_USERS){
    msg=Socket->ReceiveText();
    Log(AnsiString("Message from unknown client (")+
      IntToStr(msglen)+" bytes): '"+msg+"'");
    return;
  }
/*  int pos=0;
  while(Socket->ReceiveLength()==0 && pos<10){
    Application->ProcessMessages();
    pos++;
  }*/
  if(msglen==0){
    Log("Incoming null message from client.");
    return;
  }
  msg=Socket->ReceiveText();
  Log(AnsiString("Message from client (")+
    IntToStr(msglen)+" bytes): '"+msg+"'");
  bufAll[i]=bufAll[i]+msg;
  AnsiString buf;
  int pppi;
  do{
    pppi=bufAll[i].Pos("\n");
    if(pppi){
      buf=bufAll[i].SubString(1,pppi-1);
      bufAll[i]=bufAll[i].SubString(pppi+1,bufAll[i].Length()-pppi);
    }else break;//buf=bufAll;
  if(buf=="")break;
//  Memo1->Lines->Add(users[i].Name + ": "+buf);
  AnsiString left,right;
  int p;
  if((p=buf.Pos("="))!=0){
    left=buf.SubString(1,p-1);
    right=buf.SubString(p+1,buf.Length()-p);
  }else{left=buf;right="";}

  if(left=="NAME"){
    users[i].Name=right;
    UpdateUserInformation(i);
    for(int j=0;j<MAX_USERS;j++)
      if(users[j].used && users[j].Socket!=Socket)
        SendMsg(users[j].Socket,
                AnsiString("MOD_OPPONENT=")+users[i].id+","+users[i].Name);
  }else if(left=="PLAY")
  {
    if(StrToInt(right)<MAX_COMPS){ // play with computer
      int opp=StrToInt(right);
      susers[opp].Status=STATUS_PLAYING;
      susers[opp].used=true;
      susers[opp].opp_id=users[i].id;
      memset(xxx[opp],0,sizeof(xxx[opp]));
      memset(ooo[opp],0,sizeof(ooo[opp]));
      memset(ppp[opp],0,sizeof(ppp[opp]));
      movecount[opp]=0;
      users[i].Status=STATUS_PLAYING;
      users[i].opp_id=opp;
      UpdateUserInformation(i);
      SendMsg(users[i].Socket,"PLAY_OK=1F");
      Turns[opp]=OPP_TURN;
      for(int j=0;j<MAX_USERS;j++)
        if(users[j].used && j!=i){
          SendMsg(users[j].Socket,AnsiString("DEL_OPPONENT=")+users[i].id);
          SendMsg(users[j].Socket,AnsiString("DEL_OPPONENT=")+opp);
       }
    }else{ // play with real opponent
      int opp=FindUserByID(StrToInt(right));
      if(opp<MAX_USERS)
        if(users[opp].Status==STATUS_CONNECTED||users[opp].Status==STATUS_WAITING)
        {
          users[i].Status=STATUS_WAITING;
          users[i].opp_id=users[opp].id;
          UpdateUserInformation(i);
          if(users[opp].Status==STATUS_WAITING&&users[opp].opp_id==users[i].id){
            users[i].Status=STATUS_PLAYING;
            users[opp].Status=STATUS_PLAYING;
            UpdateUserInformation(i);
            UpdateUserInformation(opp);
            SendMsg(users[opp].Socket,"PLAY_OK=1F");
            SendMsg(users[i].Socket,"PLAY_OK=2S");
            for(int j=0;j<MAX_USERS;j++)
              if(users[j].used && j!=i && j!=opp){
                SendMsg(users[j].Socket,AnsiString("DEL_OPPONENT=")+users[i].id);
                SendMsg(users[j].Socket,AnsiString("DEL_OPPONENT=")+users[opp].id);
              }
          }else
            SendMsg(users[opp].Socket,AnsiString("PLAY=")+users[i].id);
        }else{
          SendMsg(users[i].Socket,"PLAY_CANCEL");
        }
    }
  }else if(left=="PLAY_CANCEL")
  {
    if(users[i].Status!=STATUS_CONNECTED)
    {
      if(users[i].opp_id<MAX_COMPS){
        int a=users[i].opp_id;
        susers[a].Status=STATUS_CONNECTED;
        susers[a].used=false;
        for(int j=0;j<MAX_USERS;j++)
          if(users[j].used && j!=i){
            SendMsg(users[j].Socket,AnsiString("ADD_OPPONENT=")+users[i].id+","+users[i].Name);
            SendMsg(users[j].Socket,AnsiString("ADD_OPPONENT=")+a+",Computer "+IntToStr(a+1));
          }
      }else{
        int a=FindUserByID(users[i].opp_id);
        if(a<MAX_USERS){
          if(users[i].Status==STATUS_PLAYING){
            users[a].Status=STATUS_CONNECTED;
            UpdateUserInformation(a);
            SendMsg(users[a].Socket,"PLAY_CANCEL");
            for(int j=0;j<MAX_USERS;j++)
              if(users[j].used && j!=i && j!=a){
                SendMsg(users[j].Socket,AnsiString("ADD_OPPONENT=")+users[i].id+","+users[i].Name);
                SendMsg(users[j].Socket,AnsiString("ADD_OPPONENT=")+users[a].id+","+users[a].Name);
              }
          }else
            SendMsg(users[a].Socket,AnsiString("PLAY_CANCELWAIT=")+users[i].id);
        }
      }
      users[i].Status=STATUS_CONNECTED;
      UpdateUserInformation(i);
    }
  }else if(left=="TURN")
  {
    if(users[i].opp_id<MAX_COMPS){
      AnsiString x=right.SubString(1,right.Pos(",")-1);
      AnsiString y=right.SubString(right.Pos(",")+1,right.Length()-right.Pos(","));
      GetCompTurn(i,StrToInt(x),StrToInt(y));
    }else{
      SendMsg(users[FindUserByID(users[i].opp_id)].Socket,buf);
    }
  }else if(left=="START_NEW")
  {
    if(users[i].opp_id<MAX_COMPS){
      MakeCompNew(i);
      if(Turns[users[i].opp_id]==MY_TURN){
        int x=5+random(10),y=5+random(10);
        int opp=users[i].opp_id;
        ppp[opp][y][x]=2;
        set_values(i,y,x);
        Turns[opp]=OPP_TURN;
        movecount[opp]=1;
        SendMsg(users[i].Socket,AnsiString("TURN=")+(x)+","+(y));
      }
    }else{
      SendMsg(users[FindUserByID(users[i].opp_id)].Socket,buf);
    }
  }else{
    bufAll[i]="";
//    Memo1->Lines->Add(users[i].Name + ": "+buf);
    SendMsg(Socket,"Unknown command! Terminating connection.");
    Socket->Close();
    return;
  }
  }while(ppp);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::UpdateUserInformation(int n)
{
  try{
    for(int i=0;i<ListView1->Items->Count;i++){
      if(StrToInt(ListView1->Items->Item[i]->SubItems->Strings[2])==users[n].id){
        ListView1->Items->Item[i]->Caption=users[n].Addr;
        ListView1->Items->Item[i]->SubItems->Strings[0]=users[n].Name;
        ListView1->Items->Item[i]->SubItems->Strings[1]=StatusToStr(users[n].Status);
      }
    }
  }
  catch(...){}
}

void __fastcall TForm1::MakeCompTurn(int index,int x,int y)
{
  int opp=users[index].opp_id;
  Turns[opp]=OPP_TURN;
  set_values(index,y,x);
  find_moves(index,y,x);
  ppp[opp][y][x]=2; // my_fig
  movecount[opp]++;
  if(xxx[opp][y][x]>=3200){
    FindWinner(index,y,x);
  }else{
    set_values(index,y,x);
  }
  SendMsg(users[index].Socket,AnsiString("TURN=")+(x)+","+(y));
}
void __fastcall TForm1::GetCompTurn(int index, int x, int y)
{
  int opp=users[index].opp_id;
  Turns[opp]=MY_TURN;
  movecount[opp]++;
  ppp[opp][y][x]=1; // oppfig
  if(ooo[opp][y][x]>=1600){
    FindWinner(index,x,y);
  }else if(movecount[opp]<400)
    MakeCompTurn(index,x,y);
}
void __fastcall TForm1::MakeCompNew(int index)
{
  int opp=users[index].opp_id;
  movecount[opp]=0;
  memset(xxx[opp],0,sizeof(xxx[opp]));
  memset(ooo[opp],0,sizeof(ooo[opp]));
  memset(ppp[opp],0,sizeof(ppp[opp]));
/*  for(int i=0;i<20;i++)
    for(int j=0;j<20;j++){
      ppp[opp][i][j]=xxx[opp][i][j]=ooo[opp][i][j]=0;
    }*/
}
int __fastcall TForm1::FindWinner(int index,int j, int i)
{
  int a,b,k,m,n,p;
  int asdf=users[index].opp_id;
  p=ppp[asdf][i][j];
  i++;j++;
  for(n=1;n<=4;n++)
  {
    k=0;
    for(m=-4;m<=4;m++)
    {
      a=i+m*idir[n-1];
      b=j+m*jdir[n-1];
      if(a>=1 && a<=20 && b>=1 && b<=20 ){
        if(ppp[asdf][a-1][b-1]==p){
          k=k+1;
          if(k>=5)
            return p;
        }else k=0;
      }
    }
  }
  return 0;
}

void __fastcall TForm1::N1Click(TObject *Sender)
{
   TrayIcon1->Restore();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N6Click(TObject *Sender)
{
  Close();        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N3Click(TObject *Sender)
{
  Button1->Click();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::N4Click(TObject *Sender)
{
  Button2->Click();        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ActionStartExecute(TObject *Sender)
{
  Log("Starting...");
  try{
    ServerSocket1->Open();
  }
  catch(Exception &E)
  {
    if(E.Message.Pos(" (10048),"))
      ShowMessage("Опля!\nНевозможно запустить сервер.\nВозможно он уже запущен на этом компьютере.");
    else
      ShowMessage(AnsiString(E.ClassName())+ E.Message);
    return;
  }
  if(ServerSocket1->Active)
  {
    Log("Started!");
    TrayIcon1->IconIndex=0;
    ActionStart->Enabled=false;
    ActionStop->Enabled=true;
    Caption="GoServer - Работает";
    TrayIcon1->Hint=Caption;
  }else
  {
    Log("Not started!");
  }
  for(int i=0;i<MAX_USERS;i++)users[i].used=false;
  ListView1->Items->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ActionStopExecute(TObject *Sender)
{
  Log("Shutting down...");
  try{
    ServerSocket1->Close();
  }
  catch(Exception &E)
  {
    if(E.Message.Pos("(10048)"))
      ShowMessage("Опля!\nНевозможно запустить сервер.\nВозможно он уже запущен на этом компьютере.");
    else
      ShowMessage(AnsiString(E.ClassName())+ E.Message);
    return;
  }
  if(ServerSocket1->Active==true)
  {
    Log("Not stopped!");
  }else{
    Log("Stopped!");
    TrayIcon1->IconIndex=1;
    ActionStart->Enabled=true;
    ActionStop->Enabled=false;
    Caption="GoServer - Остановлен";
    TrayIcon1->Hint=Caption;
  }
  for(int i=0;i<MAX_USERS;i++)users[i].used=false;
  for(int i=0;i<MAX_COMPS;i++)susers[i].used=false;
  ListView1->Items->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ActionExitExecute(TObject *Sender)
{
  Log("Closing");
  Close();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Log(AnsiString a)
{
  Memo1->Lines->Add(a);
  while(Memo1->Lines->Count>512)
    Memo1->Lines->Delete(0);
}


void __fastcall TForm1::FormCanResize(TObject *Sender, int &NewWidth,
      int &NewHeight, bool &Resize)
{
  if(Mouse->CursorPos.y>Top+239+30 && Memo1->Visible==false)
    Resize=true;
  else if(Mouse->CursorPos.y>Top+239 && Memo1->Visible==true)
    Resize=true;
  else if(Mouse->CursorPos.y<Top+Height)
    Resize=true;
  else{
    Resize=false;
    if(Mouse->CursorPos.y>=Top+239)
      Mouse->CursorPos=TPoint(Mouse->CursorPos.x,Top+Height);
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormResize(TObject *Sender)
{
  if(Memo1->Height<30){
    Memo1->Visible=false;
    Height=239;
    if(Mouse->CursorPos.y>=Top+239)
      Mouse->CursorPos=TPoint(Mouse->CursorPos.x,Top+Height);
  }else
    Memo1->Visible=true;
}
//---------------------------------------------------------------------------

