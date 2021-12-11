// all time in seconds
#include "iostream"
#include <simlib.h>

// wheat
int wheatKg = 1000; 
int piecesOfDoughForRounding = 0;
int piecesOfDoughForBaking = 0;
int breadPiecesForSale = 0;

const int NM_OF_BAKERS = 5;
const int NM_OF_INGREDIENT_MIXERS = 1;
const int NM_OF_DOUGH_MIXERS = 1;
const int NM_OF_DOUGH_DIVIDER = 1;
const int NM_OF_ROUNDING_TABLES = 2;
const int NM_OF_OVENS = 2;

// zarizeni
Store Baker("Bakers", NM_OF_BAKERS);
Store IngredientMixer("Ingredient Mixer", NM_OF_INGREDIENT_MIXERS);
Store DoughMixer("Dough Mixer", NM_OF_DOUGH_MIXERS);
Store DoughDivider("Dough Divider", NM_OF_DOUGH_DIVIDER);
Store RoundingTable("Rounding Tables", NM_OF_ROUNDING_TABLES);
Store Oven("Ovens", NM_OF_OVENS);

// fronty
Queue passivatedBulkToPieces;
Queue passivatedPiecesToRoundedPieces;
Queue passivatedRoundedPiecesToBread;



// classes
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
        while(wheatKg > 60)
        {
            wheatKg -= 60; //take 60kg of wheat flour
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


int main()
{
    SetOutput("bread.dat");
    Init(0, 1000000);
    (new BulkToPiecesGener)->Activate();
    (new PiecesToRoundedPiecesGener)->Activate();
    (new RoundedPiecesToBreadGener)->Activate();
    (new StartProcesses)->Activate();
    Run();

    
    // tisk statistik
    //celk.Output();
    Baker.Output();
    IngredientMixer.Output();
    DoughMixer.Output();
    DoughDivider.Output();
    RoundingTable.Output();
    Oven.Output();

    std::cout << "Number of bread that ended in store: " << breadPiecesForSale << std::endl;
    
    return 0;
}