const
   APP_AUTHOR = 'Igor Nunes';
   APP_NAME = 'Mini Calc for Linux';
   APP_SERIES = '1';
   APP_VER = APP_SERIES + '.2.3';
   // APP_BUILD = '1768';
   APP_BUILDTIME = 'June 01 2015 - 10:19:23 PM';
   CONSOLE_TITLE = APP_NAME + ' ' + APP_SERIES;

procedure WriteAppMainInfo(x, y : word);
begin
   GotoXY(x, y);
   write(APP_NAME, ' ', APP_VER, ', by ', APP_AUTHOR);
end;


procedure BuildMainScreen(const Y : word = 16);
const X : word = 7;
begin
   TextColor(DEFCOLOR_SELECTED_TEXT);
   WriteBig('MINI CALC ', X, Y);
   WriteXY(X+56, Y+3, 'for');
   WriteXY(X+56, Y+4, 'Linux');
   TextColor(DEFCOLOR_SELECTED_BACK);
   WriteBig('1', X+65, Y);
   WriteXY(1, Y+6, PadCenter('The minimalist calculator', SCREENWIDTH), 15);
   TextColor(7);
end;
