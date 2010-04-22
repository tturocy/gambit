#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <exception>
#include "stplugin.h"
#include "../libgambit/libgambit.h"

// Exception raised on convering a string to a numeric value, when the
// string does not contain a valid representation of the appropriate type.
class ValueError : public std::exception {
public:
  ValueError(const std::string &s) : m_what(s) { }
  virtual ~ValueError() throw() { }
  const char *what() const throw() { return m_what.c_str(); }

private:
  std::string m_what;
};

// Convert a string to integer, raising a ValueError exception if the
// string does not contain a valid integer representation
int gambit_atoi(const char *s)
{
  std::string ss(s);
  std::istringstream is(ss);
  is.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  int x;
  try {
    is >> x;
  }
  catch (std::ifstream::failure e) {
    throw ValueError(std::string(s) + " not a valid integer value");
  }
  if (!is.eof()) {
    throw ValueError(std::string(s) + " not a valid integer value");
  }

  return x;
}

// Convert a string to double, raising a ValueError exception if the
// string does not contain a valid integer representation
double gambit_atof(const char *s)
{
  std::string ss(s);
  std::istringstream is(ss);
  is.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  double x;
  try {
    is >> x;
  }
  catch (std::ifstream::failure e) {
    throw ValueError(std::string(s) + " not a valid floating-point value");
  }
  if (!is.eof()) {
    throw ValueError(std::string(s) + " not a valid floating-point value");
  }

  return x;
}


// Wrapper for SF_display() to support printf()-like arguments.
void stata_display(const char *format, ...)
{
  char buffer[2048];

  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  SF_display(buffer);
  va_end(args);
}

// Wrapper for SF_error() to support printf()-like arguments.
void stata_error(const char *format, ...)
{
  char buffer[2048];

  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  SF_error(buffer);
  va_end(args);
}

// Wrapper for SF_scal_save() for const-correctness
inline ST_retcode stata_scal_save(const char *scal, double v)
{
  return SF_scal_save(const_cast<char *>(scal), v);
}

#include "../tools/logit/nfglogit.h"

class STATATracer : public StrategicQREPathTracer {
public:
  STATATracer(const MixedStrategyProfile<double> &p_start)
    : StrategicQREPathTracer(p_start), 
      m_mleLambda(-1.0), m_mleLogLike(0.0), m_mleProfile(p_start) { }
  virtual ~STATATracer() { }

  double GetMLELambda(void) const { return m_mleLambda; }
  double GetMLELogLike(void) const { return m_mleLogLike; }
  const MixedStrategyProfile<double> &GetMLEProfile(void) const
  { return m_mleProfile; }

protected:
  virtual void OnStep(const Vector<double> &, bool);

private:
  double m_mleLambda, m_mleLogLike;
  MixedStrategyProfile<double> m_mleProfile;
};

void STATATracer::OnStep(const Vector<double> &p_point, bool p_isTerminal)
{
  double logL = 0.0;
  
  for (int i = 1; i <= GetMLEFrequencies().Length(); i++) {
    // Recall that the point is already expressed as log-probabilities
    logL += GetMLEFrequencies()[i] * p_point[i];
  }

  if (m_mleLambda < 0.0 || logL > m_mleLogLike) {
    m_mleLambda = p_point[p_point.Length()];
    m_mleLogLike = logL;
    for (int i = 1; i <= m_mleProfile.Length(); i++) {
      m_mleProfile[i] = exp(p_point[i]);
    }
  }
}

ST_retcode compute_qre(Gambit::List<Gambit::Game> &handles, int argc, char *argv[])
{
  double maxLambda = 1000000.0;

  Gambit::Game game = handles[atoi(argv[1])];

  if (argc != game->MixedProfileLength() + 2) {
    stata_error("qre requires one probability or frequency for each strategy.\n");
    return (ST_retcode) 198;
  }

  Gambit::MixedStrategyProfile<double> start(game);
  Gambit::Array<double> frequencies(game->MixedProfileLength());
  for (int i = 1; i <= frequencies.Length(); i++) {
    try {
      frequencies[i] = gambit_atof(argv[i+1]);
    }
    catch (ValueError e) {
      stata_error("invalid floating-point number '%s'.\n", argv[i+1]);
      return (ST_retcode) 198;
    }
  }

  STATATracer tracer(start);
  tracer.SetMLEFrequencies(frequencies);
  tracer.TraceStrategicPath(start, 0.0, maxLambda, 1.0);

  stata_scal_save("_lambda", tracer.GetMLELambda());
  stata_scal_save("_logL", tracer.GetMLELogLike());
  for (int pl = 1; pl <= game->NumPlayers(); pl++) {
    GamePlayer player = game->GetPlayer(pl);
    for (int st = 1; st <= player->NumStrategies(); st++) {
      std::ostringstream os;
      os << "_prob_" << pl << "_" << st;
      stata_scal_save(os.str().c_str(), tracer.GetMLEProfile()[player->GetStrategy(st)]);
    }
  }

  return (ST_retcode) 0;
}

//
// Create a game with the specified dimensions
//
ST_retcode create_game(Gambit::List<Gambit::Game> &handles, int argc, char *argv[])
{
  if (argc < 3) {
    stata_error("requires at least two dimensions for players\n");
    return (ST_retcode) 198;
  }

  Gambit::Array<int> dim(argc-1);
  for (int pl = 1; pl <= dim.Length(); pl++) {
    try {
      dim[pl] = gambit_atoi(argv[pl]);
    }
    catch (ValueError e) {
      stata_error("invalid number of strategies '%s' for player '%d'.\n",
		  argv[pl], pl);
      return (ST_retcode) 198;
    }

    if (dim[pl] <= 1) {
      stata_error("player '%d' must have at least 2 strategies.\n",
		  pl, dim[pl]);
      return (ST_retcode) 198;
    }
  }

  Gambit::Game game = Gambit::NewTable(dim);
  handles.Append(game);
  stata_scal_save("_newhandle", handles.Length());
  return (ST_retcode) 0;
}

//
// Load a game from a savefile at the specified path.
// If successful, store the game in the directory of games in 'handles',
// and set the first observation in the first variable to the handle number.
//
// If no variables are specified, or the variables have zero length,
// an error is raised, and the game is not saved to the handles list.
//
ST_retcode load_game(Gambit::List<Gambit::Game> &handles, char *path)
{
  Gambit::Game game;

  try  {
    std::ifstream f(path);
    game = Gambit::ReadGame(f);
  }
  catch (...) {
    stata_error("An error occurred in loading the game.\n");
    return (ST_retcode) 198;
  }

  handles.Append(game);
  stata_scal_save("_newhandle", handles.Length());
  return (ST_retcode) 0;
}

ST_retcode count_players(const Gambit::Game &p_game)
{
  stata_scal_save("_countplayers", p_game->NumPlayers());
  return (ST_retcode) 0;
}

ST_retcode count_strategies(Gambit::List<Gambit::Game> &handles,
			    int argc, char *argv[])
{
  if (argc != 3) {
    stata_error("Exactly 2 arguments required for strategies.\n");
    return (ST_retcode) 198;
  }

  Gambit::Game game = handles[atoi(argv[1])];

  int pl;

  try {
    pl = gambit_atoi(argv[2]);
  }
  catch (ValueError e) {
    stata_error("invalid player index '%s'.\n", argv[2]);
    return (ST_retcode) 198;
  }

  if (pl < 1 || pl > game->NumPlayers()) {
    stata_error("game has '%d' players; player '%d' specified.\n",
		game->NumPlayers(), pl);
    return (ST_retcode) 198;
  }

  stata_scal_save("_countstrategies", game->GetPlayer(pl)->NumStrategies());
  return (ST_retcode) 0;
}

//
// Return the payoff at a specific contingency
//
ST_retcode get_payoff(Gambit::List<Gambit::Game> &handles, int argc, char *argv[])
{
  Gambit::Game game = handles[atoi(argv[1])];

  if (argc != game->NumPlayers() + 3) {
    stata_error("Game with %d players requires %d arguments to getpayoff.\n",
		game->NumPlayers(), game->NumPlayers() + 1);
    return (ST_retcode) 198;
  }

  int pay_pl;

  try {
    pay_pl = gambit_atoi(argv[2]);
  }
  catch (ValueError e) {
    stata_error("invalid player index '%s'.\n", argv[2]);
    return (ST_retcode) 198;
  }

  if (pay_pl < 1 || pay_pl > game->NumPlayers()) {
    stata_error("game has '%d' players; player '%d' specified.\n",
		game->NumPlayers(), pay_pl);
    return (ST_retcode) 198;
  }

  Gambit::PureStrategyProfile profile(game);

  for (int pl = 1; pl <= game->NumPlayers(); pl++) {
    int st;
    try {
      st = gambit_atoi(argv[pl+2]);
    }
    catch (ValueError e) {
      stata_error("invalid strategy index '%s' for player '%d'.\n",
		  argv[pl], pl);
      return (ST_retcode) 198;
    }

    if (st < 1 || st > game->GetPlayer(pl)->NumStrategies()) {
      stata_error("player %d has %d strategies; strategy '%d' specified.\n",
		  pl, game->GetPlayer(pl)->NumStrategies(), st);
      return (ST_retcode) 198;
    }

    profile.SetStrategy(game->GetPlayer(pl)->GetStrategy(st));
  }

  stata_scal_save("_payoff", profile.GetPayoff<double>(game->GetPlayer(pay_pl)));
  return (ST_retcode) 0;
}

//
// Set the payoff at a specific contingency
//
ST_retcode set_payoff(Gambit::List<Gambit::Game> &handles, int argc, char *argv[])
{
  Gambit::Game game = handles[atoi(argv[1])];

  if (argc != game->NumPlayers() + 4) {
    stata_error("Game with %d players requires %d arguments to setpayoff.\n",
		game->NumPlayers(), game->NumPlayers() + 2);
    return (ST_retcode) 198;
  }

  int pay_pl;

  try {
    pay_pl = gambit_atoi(argv[2]);
  }
  catch (ValueError e) {
    stata_error("invalid player index '%s'.\n", argv[2]);
    return (ST_retcode) 198;
  }

  if (pay_pl < 1 || pay_pl > game->NumPlayers()) {
    stata_error("game has '%d' players; player '%d' specified.\n",
		game->NumPlayers(), pay_pl);
    return (ST_retcode) 198;
  }

  Gambit::PureStrategyProfile profile(game);

  for (int pl = 1; pl <= game->NumPlayers(); pl++) {
    int st;
    try {
      st = gambit_atoi(argv[pl+3]);
    }
    catch (ValueError e) {
      stata_error("invalid strategy index '%s' for player '%d'.\n",
		  argv[pl], pl);
      return (ST_retcode) 198;
    }

    if (st < 1 || st > game->GetPlayer(pl)->NumStrategies()) {
      stata_error("player %d has %d strategies; strategy '%d' specified.\n",
		  pl, game->GetPlayer(pl)->NumStrategies(), st);
      return (ST_retcode) 198;
    }

    profile.SetStrategy(game->GetPlayer(pl)->GetStrategy(st));
  }

  Gambit::GameOutcome outcome = profile.GetOutcome();
  if (outcome == 0) {
    outcome = game->NewOutcome();
    profile.SetOutcome(outcome);
  }

  outcome->SetPayoff(pay_pl, Gambit::Number(argv[3]));
  return (ST_retcode) 0;
}

//
// List all the games stored in the directory of games 'handles'.
//
// Does not raise any errors.
//
ST_retcode list_games(Gambit::List<Gambit::Game> &handles)
{
  for (int i = 1; i <= handles.Length(); i++) {
    stata_display("%2d: %s\n", i, handles[i]->GetTitle().c_str());
  }
  return (ST_retcode) 0;
}

Gambit::List<Gambit::Game> handles;

//
// Main dispatcher routine.  Supports the following commands:
//
// load      load a game from a Gambit savefile and store in a handle
// list      print a list of all games loaded
//
STDLL stata_call(int argc, char *argv[])
{
  if (argc == 0)  {
    stata_error("at least one option must be specified\n");
    return ((ST_retcode) 198);
  }

  if (!strcmp(argv[0], "load")) {
    if (argc == 1) {
      stata_error("load command requires path to game file\n");
      return ((ST_retcode) 198);
    }
    return load_game(handles, argv[1]);
  }
  else if (!strcmp(argv[0], "create")) {
    if (argc == 1) {
      stata_error("create command requires arguments\n");
      return ((ST_retcode) 198);
    }
    return create_game(handles, argc, argv);
  }
  else if (!strcmp(argv[0], "list")) {
    return list_games(handles);
  }
  else if (!strcmp(argv[0], "players")) {
    if (argc == 1) {
      stata_error("players command requires handle argument\n");
      return ((ST_retcode) 198);
    }
    return count_players(handles[atoi(argv[1])]);
  }
  else if (!strcmp(argv[0], "strategies")) {
    if (argc == 1) {
      stata_error("strategies command requires handle argument\n");
      return ((ST_retcode) 198);
    }
    return count_strategies(handles, argc, argv);
  }
  else if (!strcmp(argv[0], "qre")) {
    if (argc == 1) {
      stata_error("qre command requires handle argument\n");
      return ((ST_retcode) 198);
    }
    return compute_qre(handles, argc, argv);
  }    
  else if (!strcmp(argv[0], "getpayoff")) {
    if (argc == 1) {
      stata_error("getpayoff command requires handle argument\n");
      return ((ST_retcode) 198);
    }
    return get_payoff(handles, argc, argv);
  }
  else if (!strcmp(argv[0], "setpayoff")) {
    if (argc == 1) {
      stata_error("setpayoff command requires handle argument\n");
      return ((ST_retcode) 198);
    }
    return set_payoff(handles, argc, argv);
  }
  else {
    stata_error("unknown option specified\n");
    return ((ST_retcode) 198);
  }

  return 0;
}
