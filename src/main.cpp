/**
 * @file main.cpp
 * @author Adam Gajda (xgajda07@stud.fit.vutbr.cz), Jan Doležel (xdolez81@stud.fit.vutbr.cz)
 * @brief Simulation of bakery and it's supply chain with SIMLIB
 * 
 * @date 2021-12-12
 */

/**
 * @brief LIBRARIES
 * 
 */
#include "simlib.h"
#include <getopt.h>

/**
 * @brief MACROS
 * 
 */
#define WHEAT_GENERATOR 10
#define WHEAT_THRESHER 22
#define WHEAT_MILL 1
#define BAKER 5
#define INGREDIENT_MIXER 1
#define DOUGH_MIXER 1
#define DOUGH_DIVIDER 1
#define ROUNDING_TABLE 2
#define OVEN 2
#define SHOPKEEPER 1

#define CUSTOMER 20 // in minutes
#define WHEAT 345600 // eight 30day months in minutes

Store wheat_thresher("Wheat thresher", WHEAT_THRESHER);
Store wheat_mill("Wheat mill", WHEAT_MILL);
Store baker("Baker", BAKER);
Store ingredient_mixer("Ingredient mixer", INGREDIENT_MIXER);
Store dough_mixer("Dough mixer", DOUGH_MIXER);
Store rounding_table("Rounding table", ROUNDING_TABLE);
Store oven("Oven", OVEN);
Store shopkeeper("Shopkeeper", SHOPKEEPER);

Facility dough_divider("Dough divider");

class Supply_chain : public Process
{
    void Behavior()
    {
        return;
    }
};

class Bakery : public Process
{
    void Behavior()
    {
        return;
    }
};

class Generator_wheat : public Event
{
    void Behavior()
    {
        (new Supply_chain)->Activate();
        Activate(Time + WHEAT);
    }
};

class Generator_customer : public Event
{
    void Behavior()
    {
        (new Bakery)->Activate();
        Activate(Time + Exponential(CUSTOMER));
    }
};

/**
 * @brief Main body of simulation
 * 
 * @param argc num of arguments
 * @param argv argument values
 * @return int return code
 */
int main(int argc, char *argv[])
{
    return 0;
}