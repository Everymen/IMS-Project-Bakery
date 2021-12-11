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
#include "iostream"

/**
 * @brief MACROS
 * 
 */
#define SECOND * c_SECOND
#define MINUTE * c_MINUTE
#define HOUR * c_HOUR
#define DAY * c_DAY
#define MONTH * c_MONTH
#define YEAR * c_YEAR


/**
 * @brief GLOBALS
 * 
 */
unsigned long long wheat_plants = 0;
unsigned long long wheat_grains = 0;
unsigned long long plant_matter = 0;
unsigned long long flourKg = 0; 
unsigned long long piecesOfDoughForRounding = 0;
unsigned long long piecesOfDoughForBaking = 0;
unsigned long long breadPiecesForSale = 0;
unsigned long long bread_rustic = 0;
unsigned long long bread_roll = 0;
unsigned long long bread_french = 0;

const unsigned long long c_SECOND = 1;
const unsigned long long c_MINUTE = c_SECOND * 60;
const unsigned long long c_HOUR = c_MINUTE * 60;
const unsigned long long c_DAY = c_HOUR * 24;
const unsigned long long c_MONTH = c_DAY * 30;
const unsigned long long c_YEAR = c_MONTH * 12;

const unsigned long long GEN_CUSTOMER = 20 MINUTE; // 20 min in seconds
const unsigned long long GEN_WHEAT = 8 MONTH; // eight 30day months in seconds
const unsigned long long GEN_WHEAT_WAIT = 4 MONTH; // four 30day months in seconds
const unsigned long long NM_OF_ACRES = 10;
const unsigned long long NM_OF_THRESHERS = 22;
const unsigned long long NM_OF_THRESHERS = 1;
const unsigned long long NM_OF_SHOPKEEPERS = 1;
const unsigned long long NM_OF_BAKERS = 5;
const unsigned long long NM_OF_INGREDIENT_MIXERS = 1;
const unsigned long long NM_OF_DOUGH_MIXERS = 1;
const unsigned long long NM_OF_DOUGH_DIVIDER = 1;
const unsigned long long NM_OF_ROUNDING_TABLES = 2;
const unsigned long long NM_OF_OVENS = 2;

Store Baker("Bakers", NM_OF_BAKERS);
Store IngredientMixer("Ingredient Mixer", NM_OF_INGREDIENT_MIXERS);
Store DoughMixer("Dough Mixer", NM_OF_DOUGH_MIXERS);
Store DoughDivider("Dough Divider", NM_OF_DOUGH_DIVIDER);
Store RoundingTable("Rounding Tables", NM_OF_ROUNDING_TABLES);
Store Oven("Ovens", NM_OF_OVENS);
Store Wheat_thresher("Wheat thresher", NM_OF_THRESHERS);
Store Wheat_mill("Wheat mill", NM_OF_THRESHERS);
Store Shopkeeper("Shopkeeper", NM_OF_SHOPKEEPERS);

Facility Farmer("Farmer");

Queue passivatedBulkToPieces;
Queue passivatedPiecesToRoundedPieces;
Queue passivatedRoundedPiecesToBread;


class WheatProcessing : public Process{
    void Behavior(){
        while(wheat_plants >= 233)
        {
            if(!Wheat_thresher.Full())
            {
                Enter(Wheat_thresher, 1);
                wheat_plants -= 233;
                Wait(1 DAY);
                plant_matter += 133;
                wheat_grains += 100;
                Leave(Wheat_thresher);
            }
        }
    }
};

class Plant_matter : public Process{
    void Behavior(){
        while(plant_matter >= 5)
        {
            if(!Farmer.Busy())
            {
                Seize(Farmer);
                Wait(1 DAY);
                plant_matter -= 5;
                Release(Farmer);
            }
        }
    }
};

class Wheat_grains : public Process{
    void Behavior(){
        while(wheat_grains >= 50)
        {
            if(!Wheat_mill.Full())
            {
                Enter(Wheat_mill);
                Wait(1 HOUR);
                flourKg += 50;
                Leave(Wheat_mill);
            }
        }
    }
};

class Bakery : public Process{
    void Behavior(){
        return;
    }
};

class BulkToPieces : public Process{
    void Behavior(){
        //double prichod = Time;
        while(1)
        {
            if(!Baker.Full() && !IngredientMixer.Full())  // there is free Baker and IngredientMixer
            {
                break;
            }
            else
            {
                passivatedBulkToPieces.Insert(this);
                Passivate();
            }
        }
        Enter(IngredientMixer, 1);
        Enter(Baker, 1);
        Wait(Uniform(240, 300)); // 4-5min Put ingredients into mixer
        Leave(Baker, 1);
        Wait(540); // 9min Mixing ingredients
        Enter(Baker, 1);
        Wait(Uniform(60, 120)); // 1-2min Put dough out of mixer
        Leave(IngredientMixer, 1);
        Wait(Uniform(120, 180)); // 2-3min Prepare dough for fermentation
        Leave(Baker, 1);
        Wait(3600); // 1hour Dough fermentation
        Enter(Baker, 1);
        Wait(Uniform(60, 120)); // 1-2min Dough transfer
        Enter(DoughMixer, 1);
        Wait(Uniform(60, 120)); // 1-2min Mixer filling
        Leave(Baker, 1);
        Wait(300); // 5min Dough mixing
        Enter(Baker, 1);
        Wait(Uniform(60, 120)); //1-2min Dough out of mixer
        Leave(DoughMixer, 1);
        Wait(Uniform(120, 180)); // 2-3min Dough transfer
        Enter(DoughDivider, 1);
        Wait(Uniform(60, 120)); // 1-2min Dough into divider
        Leave(Baker, 1);
        for(int i=0; i<60; i++)
        {
            Wait(30); // 30s to make in piece of dough for rounding in divider
            piecesOfDoughForRounding++;
        }
        Leave(DoughDivider, 1);
        //celk(Time - prichod);
    } // end Behavior

};  // end BulkToPieces

class PiecesToRoundedPieces : public Process{
    void Behavior(){
        //double prichod = Time;
        if(!Baker.Full() && !RoundingTable.Full())
        {
            Enter(Baker, 1);
            Enter(RoundingTable, 1);
            // check if there is still another place at rounding table and free baker (if so then start another of this processes)
            if(!Baker.Full() && !RoundingTable.Full())
                (new PiecesToRoundedPieces)->Activate();
            while(piecesOfDoughForRounding > 0)
            {
                piecesOfDoughForRounding--; // take on piece of dough
                Wait(Uniform(30, 60)); // 30-60s Dough rounding
                piecesOfDoughForBaking++; // add rounded dough to baking pieces
            }
            Leave(RoundingTable, 1);
            Leave(Baker, 1);
        }
        /*
        while(piecesOfDoughForRounding > 0)
        {
            if(!Baker.Full() && !RoundingTable.Full())  // there is free Baker and RoundingTable
            {
                break;
            }
            else
            {
                passivatedPiecesToRoundedPieces.Insert(this);
                Passivate();
            }
        }
        Enter(Baker, 1);
        Enter(RoundingTable,1);
        while(piecesOfDoughForRounding > 0) // there are still some pieces for rounding
        {
            piecesOfDoughForRounding--; // take on piece of dough
            Wait(Uniform(30, 60)); // 30-60s Dough rounding
            piecesOfDoughForBaking++; // add rounded dough to baking pieces
        }
        Leave(Baker, 1);
        Leave(RoundingTable, 1);
        */
        //celk(Time - prichod);
    } // end Behavior

};  // end PiecesToRoundedPieces

class RoundedPiecesToBread : public Process{
    void Behavior(){
        //double prichod = Time;
        while(1)
        {
            if(!Baker.Full() && !Oven.Full())  // there is free Baker and RoundingTable
            {
                break;
            }
            else
            {
                passivatedRoundedPiecesToBread.Insert(this);
                Passivate();
            }
        }
        Enter(Baker, 1);
        Enter(Oven, 1);
        Wait(Uniform(60, 120)); // 1-2min Fill oven
        Leave(Baker, 1);
        Wait(1200); // 20min Baking
        Priority = 10;  // add priotity to this precess in queues
        Enter(Baker, 1);
        Wait(30); // 30s Bread out of oven
        Leave(Oven, 1);
        Wait(Uniform(180, 240)); // 3-4min Transfer to store
        Leave(Baker, 1);
        breadPiecesForSale += 20; // add 20 breads to store, where they can be sold

        //celk(Time - prichod);
    } // end Behavior

};  // end RoundedPiecesToBread

class BulkToPiecesGener : public Event{
    void Behavior(){
        while(flourKg > 60)
        {
            flourKg -= 60; //take 60kg of wheat flour
            (new BulkToPieces)->Activate();
        }
        Activate(Time + 1); // check every hour if we didnt't get more supply of wheat flour
    }   // end Behavior
};  // end BulkToPiecesGener

class PiecesToRoundedPiecesGener : public Event{
    void Behavior(){
        if(piecesOfDoughForRounding > 0)
        {
            (new PiecesToRoundedPieces)->Activate();
        }
        Activate(Time + 1); // check every minute if there are some pieces of dough to round
    }   // end Behavior
};  // end PiecesToRoundedPiecesGener

class RoundedPiecesToBreadGener : public Event{
    void Behavior(){
        while(piecesOfDoughForBaking >= 20)    // 20 bread to fill one oven
        {
            piecesOfDoughForBaking -= 20; // take 20 rounded bread
            (new RoundedPiecesToBread)->Activate();
        }
        Activate(Time + 1);
    }   // end Behavior
};  // end RoundedPiecesToBreadGener

class StartProcesses : public Event{
    void Behavior(){
        if(!passivatedBulkToPieces.Empty()) // there is something in the Queue
            (passivatedBulkToPieces.GetFirst())->Activate();
        //if(!passivatedPiecesToRoundedPieces.Empty())
            //(passivatedPiecesToRoundedPieces.GetFirst())->Activate();
        if(!passivatedRoundedPiecesToBread.Empty())
            (passivatedRoundedPiecesToBread.GetFirst())->Activate();
        Activate(Time + 1);
    }   // end Behavior
};  // end RoundedPiecesToBreadGener

class Generator_wheat_plants : public Event
{
    void Behavior()
    {
        wheat_plants += 3045;
        (new WheatProcessing)->Activate();
        Activate(Time + GEN_WHEAT + GEN_WHEAT_WAIT);
    }
};

class Generator_plant_matter : public Event{
    void Behavior(){
        if(plant_matter >= 5)
        {
            (new Plant_matter)->Activate();
        }
        Activate(Time + 1); // check every minute if there are at least 5 kilograms of plant matter
    }   // end Behavior
};  // end Generator_plant_matter

class Generator_wheat_grains : public Event{
    void Behavior(){
        if(wheat_grains >= 50)
        {
            (new Wheat_grains)->Activate();
        }
        Activate(Time + 1); // check every minute if there are at least 50 kilograms of grain
    }   // end Behavior
};  // end Generator_wheat_grains

class Generator_customer : public Event
{
    void Behavior()
    {
        (new Bakery)->Activate();
        Activate(Time + Exponential(GEN_CUSTOMER));
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
    SetOutput("bread.dat");
    Init(0, 3 YEAR);

    for (size_t i = 0; i < NM_OF_ACRES; i++)
        (new Generator_wheat_plants)->Activate();
    (new BulkToPiecesGener)->Activate();
    (new PiecesToRoundedPiecesGener)->Activate();
    (new RoundedPiecesToBreadGener)->Activate();
    (new Generator_customer)->Activate();
    (new StartProcesses)->Activate();
    Run();

    
    // tisk statistik
    //celk.Output();
    Wheat_thresher.Output();
    Wheat_mill.Output();
    Baker.Output();
    IngredientMixer.Output();
    DoughMixer.Output();
    DoughDivider.Output();
    RoundingTable.Output();
    Oven.Output();
    Shopkeeper.Output();

    std::cout << "Number of bread that ended in store: " << breadPiecesForSale << std::endl;
    
    return 0;
}