#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <exception>
#include "stplugin.h"
#include "../libgambit/libgambit.h"

//==================================================================================
//                    Safe conversions from strings to numbers
//==================================================================================

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

//==================================================================================
//          Interaction with STATA engine, converting errors to exceptions
//==================================================================================

class STATAError : public std::exception {
public:
  STATAError(const std::string &s) : m_what(s) { }
  virtual ~STATAError() throw() { }
  const char *what() const throw() { return m_what.c_str(); }

private:
  std::string m_what;
};


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

// Wrapper for SF_scal_save() to convert return code errors into exceptions
inline void stata_scal_save(const char *scal, double v)
{
  int ret = 0;
  ret = SF_scal_save(const_cast<char *>(scal), v);
  if (ret != 0)  {
    throw STATAError(std::string("failed to save value in scalar '") +
		     std::string(scal) + 
		     std::string("'"));
  }
}


//==================================================================================
//                         Interface to QRE tracing engine
//==================================================================================

#include "../tools/logit/nfglogit.h"

class STATAMLETracer : public StrategicQREPathTracer {
public:
  STATAMLETracer(const MixedStrategyProfile<double> &p_start)
    : StrategicQREPathTracer(p_start), 
      m_mleLambda(-1.0), m_mleLogLike(0.0), m_mleProfile(p_start) { }
  virtual ~STATAMLETracer() { }

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

void STATAMLETracer::OnStep(const Vector<double> &p_point, bool p_isTerminal)
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

// Compute the likelihood-maximizing value of lambda given a vector of choice
// frequencies
ST_retcode compute_qre_mle(const Gambit::Game &game, int argc, char *argv[])
{
  double maxLambda = 1000000.0;

  if (argc != game->MixedProfileLength() + 2) {
    stata_error("qre_mle requires one probability or frequency for each strategy.\n");
    return (ST_retcode) 198;
  }

  Gambit::MixedStrategyProfile<double> start(game);
  Gambit::Array<double> frequencies(game->MixedProfileLength());
  for (int i = 1; i <= frequencies.Length(); i++) {
    try {
      frequencies[i] = gambit_atof(argv[i+1]);
    }
    catch (ValueError &e) {
      stata_error("invalid floating-point number '%s'.\n", argv[i+1]);
      return (ST_retcode) 198;
    }
  }

  STATAMLETracer tracer(start);
  tracer.SetMLEFrequencies(frequencies);
  tracer.TraceStrategicPath(start, 0.0, maxLambda, 1.0);

  stata_scal_save("_lambda", tracer.GetMLELambda());
  stata_scal_save("_logL", tracer.GetMLELogLike());
 
  for (int pl = 1; pl <= game->NumPlayers(); pl++) {
    GamePlayer player = game->GetPlayer(pl);
    for (int st = 1; st <= player->NumStrategies(); st++) {
      std::ostringstream os;
      os << "_prob_" << pl << "_" << st;
      stata_scal_save(os.str().c_str(),
		      tracer.GetMLEProfile()[player->GetStrategy(st)]);
    }
  }

  return (ST_retcode) 0;
}

class STATALambdaTracer : public StrategicQREPathTracer {
public:
  STATALambdaTracer(const MixedStrategyProfile<double> &p_start)
    : StrategicQREPathTracer(p_start), m_lambda(0.0), m_profile(p_start) { }
  virtual ~STATALambdaTracer() { }

  double GetLambda(void) const { return m_lambda; }
  const MixedStrategyProfile<double> &GetProfile(void) const
  { return m_profile; }

protected:
  virtual void OnStep(const Vector<double> &, bool);

private:
  double m_lambda;
  MixedStrategyProfile<double> m_profile;
};

void STATALambdaTracer::OnStep(const Vector<double> &p_point, bool p_isTerminal)
{
  m_lambda = p_point[p_point.Length()];
  for (int i = 1; i <= m_profile.Length(); i++) {
    m_profile[i] = exp(p_point[i]);
  }
}


// Compute the QRE corresponding to a specified value of lambda
ST_retcode compute_qre_eval(const Gambit::Game &game, int argc, char *argv[])
{
  double targetLambda = 0.0;
  double maxLambda = 1000000.0;

  if (argc != 3) {
    stata_error("qre_eval requires target lambda parameter.\n");
    return (ST_retcode) 198;
  }
  try {
    targetLambda = gambit_atof(argv[2]);
  }
  catch (ValueError &e) {
    stata_error("invalid floating-point number '%s'.\n", argv[2]);
    return (ST_retcode) 198;
  }

  if (targetLambda < 0.0) {
    stata_error("qre_eval requires lambda parameter to be nonnegative; received '%s'.\n",
		argv[2]);
    return (ST_retcode) 198;
  }

  Gambit::MixedStrategyProfile<double> start(game);
  STATALambdaTracer tracer(start);
  tracer.SetTargetParam(targetLambda);
  tracer.TraceStrategicPath(start, 0.0, maxLambda, 1.0);

  stata_scal_save("_lambda", tracer.GetLambda());
  for (int pl = 1; pl <= game->NumPlayers(); pl++) {
    GamePlayer player = game->GetPlayer(pl);
    for (int st = 1; st <= player->NumStrategies(); st++) {
      std::ostringstream os;
      os << "_prob_" << pl << "_" << st;
      stata_scal_save(os.str().c_str(),
		      tracer.GetProfile()[player->GetStrategy(st)]);
    }
  }

  return (ST_retcode) 0;
}

//==================================================================================
//                          Defining and manipulating games
//==================================================================================

// Create a game with specified dimensions
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
    catch (ValueError &e) {
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

// Load a game from a savefile at the specified path.
// If successful, store the game in the directory of games in 'handles'.
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

// Save a game to a savefile at the specified path.
ST_retcode save_game(const Gambit::Game &game, int argc, char *argv[])
{
  if (argc != 3) {
    stata_error("Exactly 2 arguments required for save.\n");
    return (ST_retcode) 198;
  }

  try {
    std::ofstream f(argv[2]);
    game->WriteNfgFile(f);
  }
  catch (...) {
    stata_error("An error occurred in writing the game savefile '%s'.\n", argv[2]);
    return (ST_retcode) 198;
  }

  return (ST_retcode) 0;
}

// Return the number of players in a game
ST_retcode count_players(const Gambit::Game &p_game)
{
  stata_scal_save("_countplayers", p_game->NumPlayers());
  return (ST_retcode) 0;
}

// Return the number of strategies available to a player
ST_retcode count_strategies(const Gambit::Game &game, int argc, char *argv[])
{
  if (argc != 3) {
    stata_error("Exactly 2 arguments required for strategies.\n");
    return (ST_retcode) 198;
  }

  int pl;

  try {
    pl = gambit_atoi(argv[2]);
  }
  catch (ValueError &e) {
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

// Return the payoff at a contingency
ST_retcode get_payoff(const Gambit::Game &game, int argc, char *argv[])
{
  if (argc != game->NumPlayers() + 3) {
    stata_error("Game with %d players requires %d arguments to getpayoff.\n",
		game->NumPlayers(), game->NumPlayers() + 1);
    return (ST_retcode) 198;
  }

  int pay_pl;

  try {
    pay_pl = gambit_atoi(argv[2]);
  }
  catch (ValueError &e) {
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
    catch (ValueError &e) {
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

// Set the payoff at a contingency
ST_retcode set_payoff(const Gambit::Game &game, int argc, char *argv[])
{
  if (argc != game->NumPlayers() + 4) {
    stata_error("Game with %d players requires %d arguments to setpayoff.\n",
		game->NumPlayers(), game->NumPlayers() + 2);
    return (ST_retcode) 198;
  }

  int pay_pl;

  try {
    pay_pl = gambit_atoi(argv[2]);
  }
  catch (ValueError &e) {
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
    catch (ValueError &e) {
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

// List all the games stored in the directory of games 'handles'.
ST_retcode list_games(Gambit::List<Gambit::Game> &handles)
{
  for (int i = 1; i <= handles.Length(); i++) {
    stata_display("%2d: %s\n", i, handles[i]->GetTitle().c_str());
  }
  return (ST_retcode) 0;
}

Gambit::List<Gambit::Game> handles;

// Main entry point to STATA plugin, which handles dispatching method calls and
// dealing with exceptions in an informative way.
STDLL stata_call(int argc, char *argv[])
{
  if (argc == 0)  {
    stata_error("at least one option must be specified\n");
    return ((ST_retcode) 198);
  }

  try {
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

    if (argc == 1) {
      stata_error("'%s' command requires handle argument\n", argv[0]);
      return ((ST_retcode) 198);
    }
    Gambit::Game game;
    try {
      game = handles[gambit_atoi(argv[1])];
    }
    catch (Gambit::IndexException &e) {
      stata_error("invalid game handle '%s'\n", argv[1]);
      return ((ST_retcode) 198);
    }
    catch (ValueError &e) {
      stata_error(e.what());
      return ((ST_retcode) 198);
    }

    if (!strcmp(argv[0], "players")) {
      return count_players(game);
    }
    else if (!strcmp(argv[0], "strategies")) {
      return count_strategies(game, argc, argv);
    }
    else if (!strcmp(argv[0], "qre_mle")) {
      return compute_qre_mle(game, argc, argv);
    }    
    else if (!strcmp(argv[0], "qre_eval")) {
      return compute_qre_eval(game, argc, argv);
    }    
    else if (!strcmp(argv[0], "getpayoff")) {
      return get_payoff(game, argc, argv);
    }
    else if (!strcmp(argv[0], "setpayoff")) {
      return set_payoff(game, argc, argv);
    }
    else if (!strcmp(argv[0], "save")) {
      return save_game(game, argc, argv);
    }
    else {
      stata_error("unknown method '%s' specified\n", argv[0]);
      return ((ST_retcode) 198);
    }
  }
  catch (std::exception &e) {
    stata_error("uncaught exception in Gambit plugin: %s\n", e.what());
    return ((ST_retcode) 198);
  }
  catch (...) {
    stata_error("uncaught exception in Gambit plugin\n");
    return ((ST_retcode) 198);
  }

  return (ST_retcode) 0;
}
