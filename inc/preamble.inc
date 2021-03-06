// From the main source
procedure StaticMainMenu(item : word);    forward;

// App
procedure WriteAppMainInfo(x, y : word);  forward;
procedure BuildMainScreen(const Y : word = 16);  forward;

// GUI
procedure menu_calculator;    forward;
procedure menu_trigonometry;  forward;
procedure menu_list;          forward;
procedure menu_equation;      forward;
procedure menu_tools;         forward;
procedure menu_system;        forward;
procedure menu_random;        forward;
procedure menu_help;          forward;
procedure LoadUI;             forward;
procedure FreeUI;             forward;

// Calculator
function ComputeF(s : string; out answer : float) : boolean;   forward;
procedure ComputeGeneric(const INTERACT : boolean; s : string = ''); forward;
procedure SetVar(id : ShortString; v : Float);     forward;
function GetVar(id : ShortString) : Float;         forward;
function GetVarStr(id : ShortString) : string;     forward;
procedure SetVarStr(id : ShortString; v : string); forward;
procedure calc_prompt;     forward;

// Equations
procedure eq_environment;  forward;
procedure eq_editer;       forward;
{ procedure eq_graph;     forward; }
function FormatQuad(a, b, c : Double; const SHOWZERO : boolean = true) : string;    forward;
procedure form_quad;    forward;

// Lists (general)
procedure list_editer;     forward;

// Statistics
procedure stat_1var;    forward;
procedure stat_2var;    forward;
procedure stat_linreg;  forward;

// Trigonometry
procedure trig_analyse;    forward;
procedure trig_seecircle;  forward;

// Random
procedure random_sim;   forward;

// System
procedure mem_screen;   forward;

// Help
procedure help_contents;    forward;
procedure help_about(const INTERACT : boolean = true);   forward;
procedure help_shell(const WITHPAUSE : boolean);   forward;

// Plugins and Help
function PluginInitialization : byte;     forward;
procedure PluginFinalization;             forward;
