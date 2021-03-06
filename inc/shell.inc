procedure ExecShell;
const
   EQ_ERRMSG = 'ERR';
   EQ_LEN_VAR = 10;
   EQ_LEN_RES = 20;

var
   eq :
      record
         fn         : string;
         x1, x2, xs : Extended;
         saveonlist : boolean;
      end;
   i      : Extended;
   w      : word;
   count  : word;
   res    : Extended;
   resint : integer;
   list   : string;

begin
   try
   
   case ParamStr(1) of
      '--about' : help_about(false);  // False = no interaction
      '--help'  : help_shell(true);   // True  = with pause
      '-?'      : help_shell(false);  // False = without pause
      
      '-e' : ComputeGeneric(false, ParamStr(2));
      
      '-f' :
         begin
            case LowerCase(ParamStr(2)) of
               'y1','y2','y3','y4','y5','y6','y7','y8','y9','y0' :
                  eq.fn := mcFunctions[StrToInt(ParamStr(2)[2])];
            else
               eq.fn := ParamStr(2);
            end;
            
            case ParamStr(3) of
               '-x' :
                  begin
                     SetVar('x', StrToFloat(ParamStr(4)));
                     if ComputeF(eq.fn, res) then
                        writeln(Real2Str(res))
                     else
                        writeln(EQ_ERRMSG);
                  end;
               
               '-t' :
                  begin
                     eq.x1 := StrToFloat(ParamStr(4));
                     eq.x2 := StrToFloat(ParamStr(5));
                     eq.xs := StrToFloat(IfThen(
                                                 (ParamCount > 5) and (ParamStr(6) <> '+l'),
                                                 ParamStr(6),
                                                 '1.0')
                                               );
                     
                     eq.saveonlist := (ParamStr(6) = '+l') or ((ParamCount > 6) and (ParamStr(7) = '+l'));
                     
                     if eq.saveonlist then begin
                        if (ParamStr(6) = '+l') then
                           list := ParamStr(7)
                        else
                           list := ParamStr(8);
                        // rewrite list:
                        if listmgr.IDExists(list) then
                           listmgr.DeleteList(list);
                        listmgr.CreateNewList(list);
                        
                        eq.saveonlist := true;
                        writeln('Saving on list ', UpCase(list), '.');
                     end;
                     
                     writeln('Being f(x) = ', eq.fn);
                     writeln('x' : EQ_LEN_VAR, 'f(x)' : EQ_LEN_RES);
                     writeln(DupeString('-', EQ_LEN_VAR + EQ_LEN_RES));
                     
                     i := Min(eq.x1, eq.x2);
                     while (i <= Max(eq.x1, eq.x2)) do begin
                        SetVar('x', i);
                        if ComputeF(eq.fn, res) then begin
                           writeln(Real2Str(i) : EQ_LEN_VAR, Real2Str(res) : EQ_LEN_RES);
                           if eq.saveonlist then
                              listmgr.AppendToList(list, res);
                        end else
                           writeln(EQ_ERRMSG);
                        i := i + eq.xs;
                     end;
                  end;
            end;
         end;
      
      '+f1','+f2','+f3','+f4','+f5','+f6','+f7','+f8','+f9','+f0' :
         begin
            if (LowerCase(ParamStr(2)) = 'del') or (LowerCase(ParamStr(2)) = 'delete') then begin
               mcFunctions[StrToInt(ParamStr(1)[3])] := '';
               writeln('Function Y', ParamStr(1)[3], ' deleted.');
               
            end else if ComputeF(ParamStr(2), res) then begin
               mcFunctions[StrToInt(ParamStr(1)[3])] := ParamStr(2);
               writeln('Function Y', ParamStr(1)[3], ' set to ''', ParamStr(2), '''.');
               
            end else
               WriteError('Invalid function.' + CRLF);
         end;
      
      '-l' :
         begin
            list := ParamStr(3);
            if (ParamStr(2) <> 'new') and (ParamStr(2) <> 'form') and not ((ParamStr(2) = 'show') and (ParamStr(3) = 'all')) and (not listmgr.IDExists(list)) then begin
               WriteError('List doesn''t exist.' + CRLF);
               Exit;
            end;
            
            case ParamStr(2) of
               'add'    :
                  begin
                     if TryStrToInt(ParamStr(4), resint) and TryStrToFloat(ParamStr(5), res) then
                        if resint <= listmgr[list].count then
                           listmgr.AddToList(list, res, resint)
                        else
                           WriteError('Index out of range.' + CRLF)
                     else
                        WriteError('Either ' + ParamStr(4) + ' or ' + ParamStr(5) + ' is not a number.' + CRLF);
                  end;
               
               'append' :
                  begin
                     for w := 4 to ParamCount do begin
                        if TryStrToFloat(ParamStr(w), res) then begin
                           listmgr.AppendToList(list, res);
                        end else
                           writeln('   * ', ParamStr(w), ' is not a number.');
                     end;
                  end;
               
               'del'    :
                  begin
                     if ParamCount = 3 then begin
                        listmgr.DeleteList(list);
                        writeln('List ', UpCase(list), ' deleted.');
                        
                     end else begin
                        count := 0;
                        
                        for w := 4 to ParamCount do begin
                           if TryStrToInt(ParamStr(w), resint) then
                              if resint-count <= listmgr[list].count then begin
                                 listmgr.DeleteFromList(list, resint-count);
                                 Inc(count);
                              end else
                                 WriteError('Index out of range.' + CRLF)
                           else
                              writeln('   * ', ParamStr(w), ' is not a number.');
                        end;
                     end;
                  end;
               
               'new'    :
                  begin
                     if not listmgr.IDExists(list) then begin
                        listmgr.CreateNewList(list);
                        
                        if ParamCount >= 4 then begin
                           for w := 4 to ParamCount do begin
                              if TryStrToFloat(ParamStr(w), res) then
                                 listmgr.AppendToList(list, res)
                              else
                                 writeln('   * ', ParamStr(w), ' is not a number.');
                           end;
                        end;
                        
                        writeln('New list ', UpCase(list), ' created.');
                        
                     end else
                        WriteError('List already exists.' + CRLF);
                  end;
               
               'form'   :
                  begin
                     if not listmgr.IDExists(list) then
                        listmgr.CreateNewList(list);
                     
                     listmgr.ProcessFormula(list, ParamStr(4), @ComputeF, @WriteError);
                  end;
               
               'show'   :
                  if (ParamStr(2) = 'show') and (ParamStr(3) = 'all') then
                     if listmgr.ListCount = 0 then
                        writeln('There are no lists.')
                     else
                        for w := 0 to listmgr.ListCount-1 do
                           writeln(PadLeft(listmgr.Lists[w].id, 10), ': ', List2Str(listmgr.Lists[w]));
                        
            end;
            
            if not ((ParamStr(2) = 'show') and (ParamStr(3) = 'all')) then
               writeln(List2Str(listmgr[list]));  // function "List2Str" in "IO.inc"
         end;
   else
      ComputeGeneric(false, ParamStr(1));
   end;
   
   except
      on ex : Exception do
         WriteError('(' + ex.classname + ') ' + ex.message + CRLF);
   end;
   
end;
