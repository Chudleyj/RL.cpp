#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <random>
#include <set>
#include <iterator>
#include <functional>
#include <algorithm>
#include <initializer_list>

struct portfolioData
{
    double netWorth=10000;
    double liquid=10000;
    double profits;
    std::multiset <int, std::less <int> > positions;
};

struct RLdata
{
  const double alpha = 0.1;
  const double gamma = 0.95;
  int count = 0;
  int action; //0  = BUY, 1 = SELL, 2 = HOLD
  std::vector<std::string> dates;
  std::vector<double> prices;
  std::vector<double> Qbuy;
  std::vector<double> Qsell;
  std::vector<double> Qhold;
  std::vector<double> reward;

};

double getFutureReward(const RLdata &, const portfolioData &);
void sell(portfolioData &, RLdata &);
void buy(portfolioData &, RLdata &);
void epsilon_greedy(RLdata &, const portfolioData &);
double random_double();
int random_bool();
void get_CSV_data(std::vector<std::string> &, std::vector<double> &);
