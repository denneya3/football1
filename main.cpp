#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
//#include <curlpp/cURLpp.hpp>
//#include <curlpp/Options.hpp>
#include <unistd.h>
#include <time.h>

//unsigned int microseconds;

using namespace std;

class team {
public:
    double oRating;
    double dRating;
    string name;

    bool equals(team other){
        return other.name == name; //s this how you compare strings
    }
private:


};

class fb {
public:
    int score[2] = {0,0};
    team teams[2]; //home, away
    team offense;
    team defense;
    double waitMultiplier = 1;
    int half = 1;
    double time = 100 ; //100 - 0

    int * doHalf(){

        offense = teams[0];
        defense = teams[1];
        int cFlip = randomInt(0,1);
        if (cFlip == 1){
           team tempOf = offense;
           offense = defense;
           defense = tempOf;
        }
        cout << "Coin flip result is that "+ defense.name+" returns the kickoff.\n";
        kickoff();
        while (time > 0){
            usleep(1000000*waitMultiplier); //1 second

            put(offense.name+" pos.");

            play();
        }

        return score;

    }

    void put(string msg){
        cout << msg << " " << down<<"&"<<targetPosition-fieldPosition<< " @" << fieldPosition<< " "<<score[0]<<'-'<<score[1]<<" t:"<<time<<" h:"<<half<<endl;
        //cout << targetPosition << " " << fieldPosition << endl;
    }
private:


    int fieldPosition = 25; // 0 to 100 | RELATIVE TO OFFENSE!
    unsigned int down = 1;
    int targetPosition = fieldPosition+10;


    int randomInt(int min, int max){
        int r  = min + (rand() % static_cast<int>(max - min + 1));
        return r;
    }
    int randomInt(int min, int max, team bias){
        double rating = bias.oRating;
        if (defense.equals(bias)){
            rating = bias.dRating;
        }

        int r  = min + (rand() % static_cast<int>(max - min + 1));
        if (r != max){
            int addR = max+123;
            double b;
            while (addR+r > max){
                b = bias.oRating/5.0;
                addR = randomInt(0,max*b);

            }
            //cout << b << " addR:" << addR << " " << bias.name<< endl;
            r+addR;
        }

        return r;
    }

    int getOppositeFieldPosition(){ //for instance if offense fumbles and recovered by defense
        return 100-fieldPosition;
    }

    void resetDowns(){
        down = 1;
        targetPosition = fieldPosition+10;

    }

    void swapPossession(){
        team tempOf = offense;
        offense = defense;
        defense = tempOf;
        resetDowns();
    }

    void adjustScore(team tm, int amount){
        if (tm.equals(teams[0])){ //if team is the home team
            score[0] += amount;
        } else { //if team is not the home team
            score[1] += amount;
        }
    }

    void checkForScore(){

    }

    void fieldGoal(){
        adjustScore(offense, 3);
        put("FIELD GOAL "+offense.name);
        time-=3;
        kickoff();
    }

    int getTeamScore(team t){
        if (teams[0].equals(t)){
            return score[0];
        }
        return score[1];
    }

    team getOtherTeam(team t){
        if (!teams[0].equals(t)){
            return teams[0];
        }
        return teams[1];
    }

    void touchdown(team scorer){
        adjustScore(scorer, 6);

        put("TOUCHDOWN "+scorer.name);

         int ra = randomInt(0,100, scorer);//for pats and 2 point probability

        int teamsScoreDifference = getTeamScore(getOtherTeam(scorer))-getTeamScore(scorer);
        int leadScoreDifference = getTeamScore(scorer)- getTeamScore(getOtherTeam(scorer));
        if (teamsScoreDifference == 2 || teamsScoreDifference == 8 || leadScoreDifference == 1 ){ //if score difference is 2 or 8
            //also if score difference is 1

            if (ra > 50){ //successful
                adjustScore(scorer, 2);

            } else if (ra == 1){ //fumble/interception
                 int returnDistance = randomInt(0,110, defense);
                fieldPosition = getOppositeFieldPosition();
                fieldPosition += returnDistance;
                if (fieldPosition>100){
                put("2-PT TURNOVER, recovered by DEFENSE for "+ to_string(returnDistance)+" yards for 2 POINTS");
                adjustScore(getOtherTeam(scorer), 2);
            } else{
                put("2-PT TURNOVER, recovered by DEFENSE for "+ to_string(returnDistance)+" yards no score");
            }
            }

        } else if (ra == 1) { //return
            int returnDistance = randomInt(0,110, defense);
            fieldPosition = getOppositeFieldPosition();
            fieldPosition += returnDistance;
            if (fieldPosition>100){
                put("PAT MUFFED and RECOVERED by DEFENSE for "+ to_string(returnDistance)+" yards for 2 POINTS");
                adjustScore(getOtherTeam(scorer), 2);
            } else{
                put("PAT MUFFED and RECOVERED by DEFENSE for "+ to_string(returnDistance)+" yards no score");
            }
        } else if (ra <= 7){ // no good
            put("PAT is NO GOOD "+scorer.name);
        } else { //no good
            adjustScore(scorer, 1);
            put("PAT is GOOD "+scorer.name);
        }

        //kickoff:
        kickoff();
    }

    void safety(){
        adjustScore(defense, 2);
        put("SAFETY "+defense.name);
        kickoff();
    }

    void kickoff(){ // offense is kicking team, once catch swap possessions
        team kickingTeam = offense;

        if (half == 2 && getTeamScore(kickingTeam)< getTeamScore(defense) && time<=20){ //onside kick
            int kickProb = randomInt(1,10, offense);
            fieldPosition = 50;
            if (kickProb == 10){
                put("ONSIDE KICK GOOD, recovered by "+kickingTeam.name);
                resetDowns();
            } else {
                put("ONSIDE KICK recovered by "+getOtherTeam(kickingTeam).name);
                swapPossession();
            }

        } else {

            fieldPosition = 35;
            int kickDistance = randomInt(32, 70, offense);
            fieldPosition += kickDistance;
            swapPossession();
            fieldPosition = getOppositeFieldPosition(); //happens right after catch
            if (fieldPosition < 0) { //TOUCHBACK
                put(kickingTeam.name + " kick off for " + to_string(kickDistance) + " TOUCHBACK");

                fieldPosition = 25;
                resetDowns();
                return;
            }
            int returnDistance = randomInt(1, 40, offense);
            fieldPosition += returnDistance;
            resetDowns();
            put(kickingTeam.name + " kick off for " + to_string(kickDistance) + ", " + offense.name + " RETURNS for " +
                to_string(returnDistance));
            time -= returnDistance / 3;
        }
    }

    void punt(){
        //put(offense.name+" Punt.");

        //fieldPosition > 100-38 fake punt stuff?


        team puntingTeam = offense;
        int puntDistance = randomInt(25,50, offense);
        fieldPosition += puntDistance;
        swapPossession();
        fieldPosition = getOppositeFieldPosition();
        if (fieldPosition < 0){ //TOUCHBACK
            put(puntingTeam.name+" PUNTS for "+to_string(puntDistance)+" TOUCHBACK");
            fieldPosition = 25;
            resetDowns();
            return;
        }

        if (randomInt(0,100) <= 7){  //6.25%
            //punt is muffed
            if (randomInt(0,2) >= 1){
                put("PUNT MUFFED but RECOVERED");
                int returnDistance = (-10,15);
                fieldPosition+=returnDistance;
                if (fieldPosition<0){
                    safety();
                }
                //chance that it is not returned; or maybe it is returned
                resetDowns();
                return;
            }
            put("PUNT MUFFED and RECOVERED by kicking team! ");
            //chance for touchdown/return by recovering team
            fieldPosition = getOppositeFieldPosition();
            swapPossession();

            int returnDistance = randomInt(0,25);
            fieldPosition+=returnDistance;
            if (fieldPosition>=100){
                touchdown(offense);
            }

            resetDowns();
            return;
        }

        //returned
        int returnDistance = randomInt(-5, 50);
        fieldPosition+=returnDistance;
        put(puntingTeam.name+" PUNT for "+to_string(puntDistance)+", returns for "+to_string(returnDistance)+" yards");
        if (fieldPosition>=100){
            touchdown(offense);
        }

        resetDowns();
        return;
    }

    void adjustFieldPosition(int adjustment){
        fieldPosition+=adjustment;
        if (fieldPosition >= 100){
            touchdown(offense); //
            return;
        } else if (fieldPosition <= 0){
            safety();
            return;
        }

        if (fieldPosition>= targetPosition){
            resetDowns();
        } else{
            if (down == 4){
                put("Turnover on downs.");
                fieldPosition = getOppositeFieldPosition();
                swapPossession();
                return;
            }
            down++;
        }
    }

    void play(){

       if (fieldPosition>= targetPosition){ //suppoed to be happengin in adjustFieldPosition
           resetDowns();
       }

        if ((fieldPosition > 100-38) && (time<=4 || down == 4)) {
            if ( !(half==2 && time <= 18 && getTeamScore(defense)- getTeamScore(offense)>3)  ) { //if need more than field goal to
                fieldGoal();
                return;
            }
        }
        if (half == 2 && time<=4 && getTeamScore(offense) < getTeamScore(defense)){ //hail mary
            int hailMaryChance = randomInt(1,12);
            int tdHailMaryChance = randomInt(1,2);
            put("HAIL MARY attempt.");
            if (hailMaryChance == 12 ){
                if (tdHailMaryChance == 1){
                    touchdown(offense);
                    time -= 3;
                }else{
                    put("Incomplete pass.");
                    time -= 2;
                    adjustFieldPosition(0);
                }
            } else {
                time -= 2;
                adjustFieldPosition(0);
            }

        } else if (down != 4 || (down==4 && targetPosition-fieldPosition<= 2 && fieldPosition>52)) {
            int playTypeChance = randomInt(1,10, offense);

            if (getTeamScore(offense) > getTeamScore(defense) && time/down <= 3 && fieldPosition >= 10 ){ //victory foemation
                adjustFieldPosition(-2);
                time -= 2;
                put(offense.name+" sit on it");

            }else if (playTypeChance<=4 || fieldPosition>=98) { //running play
                int fumbleChance = randomInt(1, 100, offense); //<=2 for fumble
                int rand = randomInt(-3, 10); //field gain
                int divider = 1;
                if (fumbleChance <= 2) {
                    int recoveryChance = randomInt(0, 2);
                    if (recoveryChance == 2) {
                        adjustFieldPosition(rand/divider-2);
                        puts("FUMBLE but recovered by offense");
                    } else {
                        adjustFieldPosition(rand/divider);
                        fieldPosition = getOppositeFieldPosition();
                        puts("FUMBLE recovered by defense!");
                        swapPossession();
                        resetDowns();
                    }
                } else{
                    adjustFieldPosition(rand);
                }

                time -= rand / 3+1;



            } else { //passing play

                int rand = randomInt(-10, 25, offense); //pass length
                int completeStat = randomInt(0,10, offense);
                int interceptionProb = randomInt(1,100, offense); // <=2 for interception



                if (interceptionProb <= 2){
                    //adjustFieldPosition(rand);
                    if (fieldPosition+rand >= 100){
                        adjustFieldPosition(0);
                        //touchback
                        fieldPosition = 25;
                        swapPossession();
                        return;
                    }
                    fieldPosition+=rand;
                    fieldPosition = getOppositeFieldPosition();
                    swapPossession();

                    //new offense:
                    int returnDistance = randomInt(0,100, offense)-randomInt(-20,40, defense);

                    put("INTERCEPTED by "+offense.name+" and returned for "+ to_string(returnDistance));
                    adjustFieldPosition(returnDistance);
                    resetDowns();
                    return;
                }

                if (completeStat>6){ //incomplete
                    //nothing happened, incomplete pass
                    down++;
                } else { //complete
                    adjustFieldPosition(rand);
                    time -= rand / 3 + 1;
                }

            }

        } else { //punt
            punt();
        }

    }

};

int main() {
    team bills;
    bills.oRating = -5;
    bills.dRating = -5;
    bills.name = "raiders";

    team eagles;
    eagles.oRating = 4.6;
    eagles.dRating = 4.1;
    eagles.name = "chargers";





    int team1Points = 0;
    int team2Points = 0;

    int team1Highest = 0;
    int team2Highest = 0;

    int team1Wins = 0;
    int team2Wins = 0;

    for (int ga = 0; ga < 100; ga++) {
        time_t seconds;
        seconds = time(NULL);
        srand(seconds);

        fb game;
        game.teams[0] = bills;
        game.teams[1] = eagles;
        game.waitMultiplier = 0;

        for (int i = 1; i <= 2; i++) {
            game.time = 175;
            game.half = i;
            game.doHalf();
            cout << "End of half " + to_string(i) + ".\n";

        }
        if (game.score[0] == game.score[1]) {
            cout << "Overtime! \n";
        }

        team1Points+= game.score[0];
        team2Points += game.score[1];

        if (game.score[0] > game.score[1]){
            team1Wins++;
        } else {team2Wins++;}

        if (game.score[0] > team1Highest){
            team1Highest = game.score[0];
        }
        if (game.score[1] > team2Highest){
            team2Highest = game.score[1];
        }

    cout << game.teams[0].name + ": " + to_string(game.score[0]) + ",  " + game.teams[1].name + ": " +
            to_string(game.score[1]) << endl;
    }

    cout << endl << "Overall Stats:" << endl;
    cout << bills.name << " pts: "<< team1Points << ", wins: "<< team1Wins << ", high pts:" << team1Highest << endl;
    cout << eagles.name << " pts: "<< team2Points << ", wins: "<< team2Wins << ", high pts:" << team2Highest;

    return 0;
}
