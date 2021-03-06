(* === mcCalc ===
 * Ver: 1.0.5
 *  By: Igor Nunes
 * Expression parser for Mini Calc. *)

{$mode objfpc}
unit mcCalc;

interface
uses crt, sysutils, math, classes, fpexprpars;

var mcParser : TFPExpressionParser;
    mcResult : TFPExpressionResult;



implementation

function IsNumber(AValue: TExprFloat): Boolean;
begin
  result := not (IsNaN(AValue) or IsInfinite(AValue) or IsInfinite(-AValue));
end;


function fact(n : Byte) : Int64;
begin
   if n in [0, 1] then
      fact := 1
   else
      fact := n * fact(pred(n));
end;


procedure ExprTan(var Result: TFPExpressionResult; const Args: TExprParameterArray);
var
   x : Double;
begin
   x := ArgToFloat(Args[0]);
   if IsNumber(x) and (frac(x - 0.5) / pi <> 0.0) then
      Result.resFloat := tan(x)
   else
      Result.resFloat := NaN;
end;


procedure ExprPow(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Result.resFloat := math.power(ArgToFloat(Args[0]), ArgToFloat(Args[1]))
end;


procedure ExprFact(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   if (Args[0].resInteger < 0) then
      raise EExprParser.Create('Invalid range for argument 1: Should not be negative.')
   
   else if (Args[0].resInteger > 255) then
      raise EExprParser.Create('Invalid range for argument 1: Should not be greater than 255.')
   
   else
      Result.resInteger := fact(Args[0].resInteger);
end;


procedure ExprnCk(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   if (Args[0].resInteger < 0) then
      raise EExprParser.Create('Invalid range for argument 1: Should not be negative.')
   
   else if (Args[1].resInteger < 0) then
      raise EExprParser.Create('Invalid range for argument 2: Should not be negative.')
   
   else if (Args[0].resInteger > 255) then
      raise EExprParser.Create('Invalid range for argument 1: Should not be greater than 255.')
   
   else if (Args[1].resInteger > 255) then
      raise EExprParser.Create('Invalid range for argument 2: Should not be greater than 255.')
   
   else
      Result.resInteger := fact(Args[0].resInteger) div (fact(Args[1].resInteger) * (Args[0].resInteger - Args[1].resInteger));
end;


procedure ExprnPk(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   if (Args[0].resInteger < 0) then
      raise EExprParser.Create('Invalid range for argument 1: Should not be negative.')
   
   else if (Args[1].resInteger < 0) then
      raise EExprParser.Create('Invalid range for argument 2: Should not be negative.')
   
   else if (Args[0].resInteger > 255) then
      raise EExprParser.Create('Invalid range for argument 1: Should not be greater than 255.')
   
   else if (Args[1].resInteger > 255) then
      raise EExprParser.Create('Invalid range for argument 2: Should not be greater than 255.')
   
   else
      Result.resInteger := fact(Args[0].resInteger) div (Args[0].resInteger - Args[1].resInteger);
end;


procedure Exprdegtograd(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Result.resFloat := math.DegToGrad(ArgToFloat(Args[0]));
end;


procedure Exprdegtorad(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Result.resFloat := math.DegToRad(ArgToFloat(Args[0]));
end;


procedure Exprradtodeg(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Result.resFloat := math.RadToDeg(ArgToFloat(Args[0]));
end;


procedure Exprradtograd(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Result.resFloat := math.RadToGrad(ArgToFloat(Args[0]));
end;


procedure Exprgradtodeg(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Result.resFloat := math.GradToDeg(ArgToFloat(Args[0]));
end;


procedure Exprgradtorad(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Result.resFloat := math.GradToRad(ArgToFloat(Args[0]));
end;


procedure ExprRand(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Randomize;
   Result.resFloat := random();
end;


procedure ExprRandInt(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Randomize;
   Result.resInteger := math.RandomRange(Args[0].resInteger, Args[1].resInteger);
end;


procedure ExprLogN(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Result.resFloat := math.logn(ArgToFloat(Args[1]), ArgToFloat(Args[0]));
end;


procedure ExprLog10(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Result.resFloat := math.log10(ArgToFloat(Args[0]));
end;


procedure ExprLog2(var Result: TFPExpressionResult; const Args: TExprParameterArray);
begin
   Result.resFloat := math.log2(ArgToFloat(Args[0]));
end;


procedure ExprIs(var Result: TFPExpressionResult; const Args: TExprParameterArray);
const
   Bool2Int : array [boolean] of Int64 = (0, 1);
begin
   Result.resInteger := Bool2Int[Args[0].resBoolean];
end;


procedure ExprIsNot(var Result: TFPExpressionResult; const Args: TExprParameterArray);
const
   Bool2Int : array [boolean] of Int64 = (0, 1);
begin
   Result.resInteger := 1 div Bool2Int[not Args[0].resBoolean];
end;


var c : char;  // for initialization and finalization
    i : byte;

initialization
   mcParser := TFPExpressionParser.Create(nil);
   mcParser.Builtins := [bcMath, bcStrings, bcDateTime, bcBoolean, bcConversion];
   
   // Variables
   for c := 'a' to 'z' do
      mcParser.Identifiers.AddFloatVariable(c, 0.0);
   
   mcParser.Identifiers.AddFloatVariable('x1', 0.0);
   mcParser.Identifiers.AddFloatVariable('x2', 0.0);
   mcParser.Identifiers.AddFloatVariable('vx', 0.0);
   mcParser.Identifiers.AddFloatVariable('vy', 0.0);
   
   mcParser.Identifiers.AddFloatVariable('r2', 0.0);
   
   mcParser.Identifiers.AddFloatVariable('min', 0.0);
   mcParser.Identifiers.AddFloatVariable('max', 0.0);
   mcParser.Identifiers.AddFloatVariable('sum', 0.0);
   mcParser.Identifiers.AddFloatVariable('sumsqr', 0.0);
   mcParser.Identifiers.AddFloatVariable('mean', 0.0);
   mcParser.Identifiers.AddFloatVariable('variance', 0.0);
   mcParser.Identifiers.AddFloatVariable('popvariance', 0.0);
   mcParser.Identifiers.AddFloatVariable('stddev', 0.0);
   mcParser.Identifiers.AddFloatVariable('popstddev', 0.0);
   mcParser.Identifiers.AddFloatVariable('m1', 0.0);
   mcParser.Identifiers.AddFloatVariable('m2', 0.0);
   mcParser.Identifiers.AddFloatVariable('m3', 0.0);
   mcParser.Identifiers.AddFloatVariable('m4', 0.0);
   mcParser.Identifiers.AddFloatVariable('skew', 0.0);
   mcParser.Identifiers.AddFloatVariable('kurtosis', 0.0);
   
   // Protected memory (only user can use it within Memory Manager)
   for i := 1 to 10 do
      mcParser.Identifiers.AddFloatVariable('mem' + IntToStr(i), 0.0);
   
   // Constants
   mcParser.Identifiers.AddFloatVariable('ans', 0.0);
   mcParser.Identifiers.AddFloatVariable('euler$', 2.718281828459045235360287);
   
   // New strings
   mcParser.Identifiers.AddStringVariable('regeq', '');
   mcParser.Identifiers.AddStringVariable('quadeq', '');
   
   // New functions
   mcParser.Identifiers.AddFunction('tan', 'F', 'F', @ExprTan);
   mcParser.Identifiers.AddFunction('pow', 'F', 'FF', @ExprPow);
   mcParser.Identifiers.AddFunction('log10', 'F', 'F', @ExprLog10);
   mcParser.Identifiers.AddFunction('log2', 'F', 'F', @ExprLog2);
   mcParser.Identifiers.AddFunction('logn', 'F', 'FF', @ExprLogN);
   mcParser.Identifiers.AddFunction('fact', 'I', 'I', @ExprFact);
   mcParser.Identifiers.AddFunction('nCk', 'I', 'II', @ExprnCk);
   mcParser.Identifiers.AddFunction('nPk', 'I', 'II', @ExprnPk);
   mcParser.Identifiers.AddFunction('rand', 'F', '', @ExprRand);
   mcParser.Identifiers.AddFunction('randint', 'I', 'II', @ExprRandInt);
   mcParser.Identifiers.AddFunction('radtodeg' , 'F', 'F', @Exprradtodeg);
   mcParser.Identifiers.AddFunction('radtograd', 'F', 'F', @Exprradtograd);
   mcParser.Identifiers.AddFunction('degtorad' , 'F', 'F', @Exprdegtorad);
   mcParser.Identifiers.AddFunction('degtograd', 'F', 'F', @Exprdegtograd);
   mcParser.Identifiers.AddFunction('gradtodeg', 'F', 'F', @Exprgradtodeg);
   mcParser.Identifiers.AddFunction('gradtorad', 'F', 'F', @Exprgradtorad);
   mcParser.Identifiers.AddFunction('is', 'I', 'B', @ExprIs);
   mcParser.Identifiers.AddFunction('is!', 'I', 'B', @ExprIsNot);
   
   // Initialize random number generator:
   // Randomize;


finalization
   mcParser.Destroy;

end.
