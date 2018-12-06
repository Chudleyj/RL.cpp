#include "RL.h"

int main()
{
  portfolioData portfolio; //Start with 100 dollars
  RLdata RL;

  //Fill in price and date vectors from CSV file
  get_CSV_data(RL.dates,RL.prices);

  for(int t = RL.count; t < RL.prices.size()-1; t++){

    //Start the RL by choosing an action
    epsilon_greedy(RL, portfolio);
    if(RL.action !=2)
      std::cout<<RL.action << std::endl;
    //Execute chosen action

    //Hold
    if(RL.action == 2)
        RL.reward.push_back(0);
    //Buy
    else if(RL.action == 0)
        buy(portfolio, RL);
    //Sell
    else
        sell(portfolio, RL);


    //Max Q`(s`,a`)
    int futureReward = getFutureReward(RL, portfolio);

    //Action was taken, reward has been given, now update Q
    if(RL.action == 2){
      RL.Qhold.push_back(0);
      RL.Qsell.push_back(RL.Qsell[RL.count-1]);
      RL.Qbuy.push_back(RL.Qsell[RL.count-1]);
    }

    else if(RL.action == 0){
      if(RL.count == 0){
        RL.Qbuy.push_back(0 + RL.alpha *
                          (RL.reward[RL.count] + (RL.gamma *
                            futureReward) - 0));

        RL.Qsell.push_back(0);
        RL.Qhold.push_back(0);
      }


      else{
        RL.Qbuy.push_back(RL.Qbuy[RL.count-1] + RL.alpha *
                          (RL.reward[RL.count] + (RL.gamma *
                            futureReward) - RL.Qbuy[RL.count-1]));
        RL.Qsell.push_back(RL.Qsell[RL.count-1]);
        RL.Qhold.push_back(0);
      }
    }

    //Update Q for sell action
    else{
      if(RL.count == 0){
        RL.Qsell.push_back(0 + RL.alpha *
                           (RL.reward[RL.count] + (RL.gamma *
                             futureReward) - 0));
        RL.Qbuy.push_back(0);
        RL.Qhold.push_back(0);
      }
      else{
        RL.Qsell.push_back(RL.Qsell[RL.count-1] + RL.alpha *
                          (RL.reward[RL.count] + (RL.gamma *
                            futureReward) - RL.Qsell[RL.count-1]));
        RL.Qbuy.push_back(RL.Qbuy[RL.count-1]);
        RL.Qhold.push_back(0);
      }
    }
    RL.count++;
  }//End loop

  std::cout<< std::endl << std::endl << "DONE: " << portfolio.profits <<
  std::endl<< std::endl;

}

double getFutureReward(const RLdata &RL, const portfolioData &env)
{
  double sellReward = 0, buyReward = 0, holdReward=0, tempProfit =0, tempPosition = 0;

  if(env.positions.empty())
    sellReward = -1;

  else{
    std::multiset<int>::iterator itr = env.positions.begin();
    for(int i = 0; i < env.positions.size(); i++){
      tempPosition = *itr;
      tempProfit += RL.prices[RL.count+1] - tempPosition;
      itr++;
    }

    if(tempProfit > 0)
      sellReward = 1;
    else if(tempProfit < 0)
      sellReward = -1;
    else
      sellReward = 0;
  }

  if(env.liquid < RL.prices[RL.count+1])
    buyReward = -1;

  else
    buyReward = 0;

  return (std::max({sellReward, buyReward, holdReward}));
}

void sell(portfolioData &env, RLdata &RL)
{
  double tempProfit = 0, tempPosition = 0;
  std::multiset<int>::iterator itr = env.positions.begin();
  for(int i = 0; i < env.positions.size(); i++){
    tempPosition = *itr;
    tempProfit += RL.prices[RL.count] - tempPosition;
    itr++;
  }

  env.profits += tempProfit;
  env.liquid += tempProfit;
  env.netWorth = env.liquid;
  env.positions.clear();

  if(tempProfit > 0)
    RL.reward.push_back(1);
  else if(tempProfit < 0)
    RL.reward.push_back(-1);
  else
    RL.reward.push_back(0);
}

void buy(portfolioData &env, RLdata &RL)
{
  RL.reward.push_back(0);
  env.positions.insert(RL.prices[RL.count]);
  env.liquid = env.liquid - RL.prices[RL.count];
  env.netWorth = env.liquid + (env.positions.size() * RL.prices[RL.count]);
}

void epsilon_greedy(RLdata &RL, const portfolioData &env)
{
  static const auto epsilon = 0.1;

  auto  p = random_double();

  if(RL.count < 10)
    RL.action = random_bool();

  else if(p > epsilon && RL.Qbuy[RL.count] != RL.Qsell[RL.count]){
    if(RL.Qbuy[RL.count] > RL.Qsell[RL.count])
      RL.action = 0;
    else
      RL.action = 1;
  }

  else
    RL.action = random_bool();

  if(RL.action == 0 && env.liquid <RL.prices[RL.count])
    RL.action = 2;
  else if(RL.action == 1 && env.positions.empty())
    RL.action = 2;
}

double random_double()
{
  std::random_device rd;
  std::mt19937 e2(rd());
  std::uniform_real_distribution<> dist(0, 1);
  return dist(e2);
}

int random_bool()
{
  static auto gen = std::bind(
                      std::uniform_int_distribution<>(0,1),
                        std::default_random_engine());
  return gen();
}

void get_CSV_data(std::vector<std::string> &date, std::vector<double> &price)
{
  std::ifstream ip("DJI.csv");

  if(!ip.is_open())
    std::cout << "Error opening file." << std::endl;

  std::string temp;

  while(ip.good()){
    getline(ip,temp,',');
    date.push_back(temp);
    getline(ip,temp,',');
    price.push_back(atof(temp.c_str()));
  }

  ip.close();
}
