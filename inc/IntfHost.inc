type
   TPluginHost = class(TInterfacedObject, IPluginHost)
      private
         function ListByIndexCount : word;
         
      public
         function Compute(expression : WideString) : TMCResult;
         procedure SetFloatVariable(id : WideString; v : Double);
         function GetFloatVariable(id : WideString) : Double;
         function GetStringVariable(id : WideString) : WideString;
         procedure SetStringVariable(id : WideString; v : WideString);
         
         function ListExists(const ID : WideString) : boolean;
         function CreateList(id : WideString) : boolean;
         procedure DeleteList(id : WideString);
         procedure DeleteList(i : word); overload;
         
         function AppendToList(id : WideString; value : Double) : boolean;
         function AppendToList(i : LongInt; value : Double) : boolean; overload;
         function AddToList(id : WideString; value : Double; ind : word) : boolean;
         function AddToList(i : LongInt; value : Double; ind : word) : boolean; overload;
         function DeleteFromList(id : WideString; ind : word) : boolean;
         function DeleteFromList(i : LongInt; ind : word) : boolean; overload;
         
         property NumberOfLists : word read ListByIndexCount;
         
         function ListByName(const ID : WideString) : TList;
         function ListByIndex(const I : word) : TList;
         function GetListElement(const ID : WideString; const I : LongInt) : Double;
         procedure SetListElement(const ID : WideString; const I : LongInt; thevalue : Double);
         function NumberOfElementsByName(const ID : WideString) : word;
         function NumberOfElementsByIndex(const I : word) : word;
         
         procedure SetFunction(i : word; fn : WideString);
         function GetFunction(i : word) : WideString;
   end;
   
   TPluginInfo = class
      dll  : TLibHandle;
      intf : IPlugin;
   end;
   
   TMCPlugins = array of TPluginInfo;
   TPluginInit = function (host : IPluginHost) : IPlugin; stdcall;

var
   myself  : IPluginHost = nil;
   plugins : TMCPlugins;

(* Initialization and finalization *)
const
   PLUGIN_OK          = 0;  // perfect
   PLUGIN_INEXISTENT  = 1;  // no extensions... that's ok
   PLUGIN_INVALID     = 2;  // there are invalid plugins... they're ignored, so that's ok as well
   PLUGIN_INTERRUPTED = 3;  // Capacity limited reached
   PLUGIN_FATAL       = 4;  // this isn't good... program must shutdown.


function PluginInitialization : byte;

   procedure Next(var c : char);
   begin
      if c = '9' then
         c := 'A'
      else
         c := Succ(c);
   end;

const
   DIR = 'plugin/';

var
   finder : TSearchRec;
   dll    : TLibHandle;
   proc   : TPluginInit;
   plug   : TPluginInfo;
   idx    : char = '1';

begin
   PluginInitialization := PLUGIN_OK;
   
   writeln('DIR=''', GetCurrentDir + '/' + DIR, '''');
   if FindFirst(GetCurrentDir + '/' + DIR + 'libmcp*.so', faAnyFile, finder) <> 0 then begin
      PluginInitialization := PLUGIN_INEXISTENT;
      pluginmenu.Add('Back', '<-', #8, TProc(nil));
      Exit;
   end else
      writeln('   There are plugins to load.');
   
   myself := TPluginHost.Create as IPluginHost;
   
   SetLength(plugins, 0);
   repeat
      write('   > ', finder.Name, ' - ');
      dll := LoadLibrary(PChar(GetCurrentDir + '/' + DIR + finder.Name));
      
      if dll <> NilHandle then begin
         write('loaded, ');
         proc := TPluginInit(GetProcedureAddress(dll, 'PluginInit'));
         
         if Assigned(proc) then begin
            write('init procedure assigned, ');
            plug := TPluginInfo.Create;
            
            try
               plug.dll  := dll;
               plug.intf := proc(myself);
               SetLength(plugins, Succ(Length(plugins)));
               plugins[High(plugins)] := plug;
               pluginmenu.Add(plug.intf.PluginName, idx, idx, TProcPlug(@plug.intf.Execute));
               writeln('ready.');
               if idx = 'Z' then begin
                   PluginInitialization := PLUGIN_INTERRUPTED;
                   break;
               end else
                   Next(idx);
            except
               plug.Free;
               writeln('aborted.');
            end;
         end else begin
            PluginInitialization := PLUGIN_INVALID;
            writeln('invalid plugin.');
         end;
      end else
         writeln('returned ''NilHandle''.');
   until FindNext(finder) <> 0;
   
   pluginmenu.Add('Back', '<-', #8, TProc(nil));
   
   sysutils.FindClose(finder);
end;


procedure PluginFinalization;
var
   i : word;
begin
   if Length(plugins) = 0 then
      Exit;
   
   for i := Low(plugins) to High(plugins) do begin
      plugins[i].intf := nil;
      if plugins[i].dll <> NilHandle then
         if UnloadLibrary(plugins[i].dll) then
            plugins[i].dll := NilHandle;
      plugins[i].Free;
   end;
   
   SetLength(plugins, 0);
end;


(* Implementation of host methods *)

function TPluginHost.Compute(expression : WideString) : TMCResult;
const
   RT2MC : array [TResultType] of TMCResultType = (mcBoolean, mcInteger, mcFloat, mcDateTime, mcString);
var
   res : TFPExpressionResult;
begin
   mcParser.Expression := expression;
   res := mcParser.Evaluate;
   Compute.ResultType  := RT2MC[res.ResultType];
   Compute.ResString   := res.ResString;
   Compute.ResBoolean  := res.ResBoolean;
   Compute.ResInteger  := res.ResInteger;
   Compute.ResFloat    := res.ResFloat;
   Compute.ResDateTime := res.ResDateTime;
end;


procedure TPluginHost.SetFloatVariable(id : WideString; v : Double);
begin
   SetVar(id, v);
end;


function TPluginHost.GetFloatVariable(id : WideString) : Double;
begin
   GetFloatVariable := GetVar(id);
end;


function TPluginHost.GetStringVariable(id : WideString) : WideString;
begin
   GetStringVariable := GetVarStr(id);
end;


procedure TPluginHost.SetStringVariable(id : WideString; v : WideString);
begin
   SetVarStr(id, v);
end;


function TPluginHost.ListExists(const ID : WideString) : boolean;
begin
   ListExists := listmgr.IDExists(ID);
end;


function TPluginHost.CreateList(id : WideString) : boolean;
begin
   CreateList := listmgr.CreateNewList(id);
end;


procedure TPluginHost.DeleteList(id : WideString);
begin
   listmgr.DeleteList(id);
end;


procedure TPluginHost.DeleteList(i : word); overload;
begin
   listmgr.DeleteList(i);
end;


function TPluginHost.AppendToList(id : WideString; value : Double) : boolean;
begin
   AppendToList := listmgr.AppendToList(id, value);
end;


function TPluginHost.AppendToList(i : LongInt; value : Double) : boolean; overload;
begin
   AppendToList := listmgr.AppendToList(i, value);
end;


function TPluginHost.AddToList(id : WideString; value : Double; ind : word) : boolean;
begin
   AddToList := listmgr.AddToList(id, value, ind);
end;


function TPluginHost.AddToList(i : LongInt; value : Double; ind : word) : boolean; overload;
begin
   AddToList := listmgr.AddToList(i, value, ind);
end;


function TPluginHost.DeleteFromList(id : WideString; ind : word) : boolean;
begin
   DeleteFromList := listmgr.DeleteFromList(id, ind);
end;


function TPluginHost.DeleteFromList(i : LongInt; ind : word) : boolean; overload;
begin
   DeleteFromList := listmgr.DeleteFromList(i, ind);
end;


function TPluginHost.ListByIndexCount : word;
begin
   ListByIndexCount := listmgr.ListCount;
end;


function TPluginHost.ListByName(const ID : WideString) : TList;
begin
   // ListByName := mcIntf.TList(listmgr.ListsStr[ID]);
   ListByName.data := listmgr.ListsStr[ID].data;
   WideString(ListByName.id) := listmgr.ListsStr[ID].id;
   ListByName.count := listmgr.ListsStr[ID].count;
   ListByName.active := listmgr.ListsStr[ID].active;
end;


function TPluginHost.ListByIndex(const I : word) : TList;
begin
   // ListByIndex := mcIntf.TList(listmgr.Lists[I]);
   ListByIndex.data := listmgr.Lists[I].data;
   WideString(ListByIndex.id) := listmgr.Lists[I].id;
   ListByIndex.count := listmgr.Lists[I].count;
   ListByIndex.active := listmgr.Lists[I].active;
end;


function TPluginHost.GetListElement(const ID : WideString; const I : LongInt) : Double;
begin
   GetListElement := listmgr.Elements[ID, I];
end;


procedure TPluginHost.SetListElement(const ID : WideString; const I : LongInt; thevalue : Double);
begin
   listmgr.Elements[ID, I] := thevalue;
end;


function TPluginHost.NumberOfElementsByName(const ID : WideString) : word;
begin
   NumberOfElementsByName := listmgr.ListsStr[ID].count;
end;


function TPluginHost.NumberOfElementsByIndex(const I : word) : word;
begin
   NumberOfElementsByIndex := listmgr.Lists[I].count;
end;


procedure TPluginHost.SetFunction(i : word; fn : WideString);
begin
   if mcFunctions.count > Succ(i) then
      Exit;
   
   mcFunctions[i] := fn;
end;


function TPluginHost.GetFunction(i : word) : WideString;
begin
   GetFunction := '';
   if mcFunctions.count <= Succ(i) then
      GetFunction := mcFunctions[i];
end;
