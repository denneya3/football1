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

int team1AddedBias = 0;
int team2AddedBias = 0;

using namespace std;

class team {
public:
    double oRating;
    double dRating;
    string name;
    unsigned int designation = 1;

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

    bool output = true;

    void addToStat(team t, int stat){

    }

    bool isOvertime = false;

    int * doHalf(bool overtimeRules){
        isOvertime = overtimeRules;

        offense = teams[0];
        defense = teams[1];
        int cFlip = randomInt(0,1);
        if (cFlip == 1){
           team tempOf = offense;
           offense = defense;
           defense = tempOf;
        }
        put("Coin flip result is that "+ defense.name+" returns the kickoff.");
        kickoff();
        while (time > 0){
            usleep(1000000*waitMultiplier); //1 second

            put(offense.name+" pos.");

            //for overtime: it is play out until end (for equal opportunity)

            //if (overtimeRules && abs(score[0]-score[1])>=6)

            play();
        }

        return score;

    }

    void put(string msg){
        if (output==true) { // resource intensive line of code
            cout << msg << " " << down << "&" << targetPosition - fieldPosition << " @" << fieldPosition << " "
                 << score[0] << '-' << score[1] << " t:" << time << " h:" << half << endl;
            //cout << targetPosition << " " << fieldPosition << endl;
        }
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

        int maxBias = (int) ((max-r)*(rating/5.0));
        int addR = randomInt(0,maxBias);
        addR = maxBias*0.5;

        if (bias.designation == 1){
            team1AddedBias+=addR;
        } else {team2AddedBias+=addR;}

        if (false){ //old CODE!
            int addR = max+123;
            double b;
            while (addR+r >= max){
                b = bias.oRating/5.0;
                addR = randomInt(0,max*b);

            }
            //cout << b << " addR:" << addR << " " << bias.name<< endl;


        }

        r+=addR;

        //r*=(rating/5.0); //THIS IS PROBABLY BAD!!!

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
        int fieldGoalProb = randomInt(0,100,offense);
        int a = getOppositeFieldPosition()/70.0 * 60;

        if (fieldGoalProb>a) {
            adjustScore(offense, 3);
            put("FIELD GOAL MADE " + offense.name);
            time-=3;
            kickoff();
        } else {
            put("FIELD GOAL missed, TURNOVER "+offense.name);
            time-=3;
            fieldPosition = getOppositeFieldPosition();
            swapPossession();
        }
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
        if (teamsScoreDifference == 2 || teamsScoreDifference == 9 || leadScoreDifference == 1 ){ //if score difference is 2 or 8
            //also if score difference is 1

            if (ra > 50){ //successful
                adjustScore(scorer, 2);
                put(offense.name+" 2-PT try SUCCESS");
            } else if (ra == 1){ //fumble/interception
                 int returnDistance = randomInt(0,110, defense);
                fieldPosition = getOppositeFieldPosition();
                fieldPosition += returnDistance;
                if (fieldPosition>=100){
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
            if (fieldPosition>=100){
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
            int kickProb = randomInt(1,10); //decided no bias
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
            int returnDistance = randomInt(1, 40, offense); //possession swapped...
            fieldPosition += returnDistance;
            resetDowns();
            put(kickingTeam.name + " kick off for " + to_string(kickDistance) + ", " + offense.name + " RETURNS for " +
                to_string(returnDistance));
            time -= randomInt(1,4);
        }
    }

    void punt(){
        //put(offense.name+" Punt.");

        //fieldPosition > 100-38 fake punt stuff?
        int fakePuntProb = randomInt(1,500);
        if (fakePuntProb==329){
            int fakeDistance = randomInt(-50,60);
            if (fakeDistance<=0){
                fakeDistance=1;
            }

            int gain = (targetPosition-fieldPosition)+ fakeDistance;
            fieldPosition+= gain;
            put("FAKE PUNT for "+to_string(gain));
            resetDowns();
            if (fieldPosition>=100){
                touchdown(offense);
            }
            return;
        }

        team puntingTeam = offense;
        int puntDistance = randomInt(25,75, offense);
        fieldPosition += puntDistance;
        fieldPosition = getOppositeFieldPosition();
        swapPossession(); //OFFENSE IS NOW catching

        if (fieldPosition < 0){ //TOUCHBACK
            put(puntingTeam.name+" PUNTS for "+to_string(puntDistance)+" TOUCHBACK");
            fieldPosition = 25;
            resetDowns();
            return;
        }

        if (randomInt(0,100, offense) <= 6){  //6.25%, new offense
            //punt is muffed
            if (randomInt(0,2) >= 1){
                put(puntingTeam.name+" PUNT for "+to_string(puntDistance)+" MUFFED but RECOVERED");
                int returnDistance = randomInt(-10,15, offense);
                fieldPosition+=returnDistance;
                if (fieldPosition<0){
                    safety();
                }
                //chance that it is not returned; or maybe it is returned
                resetDowns();
                return;
            }
            //else: muffed and recovered by kicking team
            put(puntingTeam.name+" PUNT for "+to_string(puntDistance)+" MUFFED and RECOVERED by kicking team! ");
            //chance for touchdown/return by recovering team
            fieldPosition = getOppositeFieldPosition();
            swapPossession(); //kicking team is now offense

            int returnDistance = randomInt(0,25, offense); //new offense (switch??)
            fieldPosition+=returnDistance;
            if (fieldPosition>=100){
                touchdown(offense);
            }

            resetDowns();
            return;
        }

        //returned
        int returnDistance = randomInt(-8, 20, offense)+pow(50, (double) randomInt(0,100, offense)/105.0 );
        fieldPosition+=returnDistance;
        put(puntingTeam.name+" PUNT for "+to_string(puntDistance)+", returns for "+to_string(returnDistance)+" yards");
        if (fieldPosition>=100){
            put("PUNT RETURN TOUCHDOWN");
            touchdown(offense);
        }

        resetDowns(); //Redundant??
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

    //PENALTIES SETTINGS:
    int offensivePenalties[3] = {10, 5, 15};
    int defensivePenalties[3] = {5, 5, 15};
    bool downs[3] = {false, false, true};
    string oMessages[3] = {"Holding", "False Start", "Personal Foul"};
    string dMessages[3] = {"Holding", "Offsides", "Personal Foul"};

    void penalize(bool side, int penalty){
        // side = true, offense
        // side = false, defense
        int amount = 0;
        double penaltyFactor = 1;
        if (side == true){ // OFFENSIVE PENALTY
            amount = offensivePenalties[penalty];

            if (fieldPosition<=20){
                penaltyFactor = 0.5;
            }

            if (fieldPosition-amount <= 0){
                amount = fieldPosition/2;
            }

            if (downs[penalty]==true && down != 4){
                down++;
            }


            fieldPosition-=amount;

        } else if (side == false){ //DEFENSIVE PENALTY
            amount = defensivePenalties[penalty];

            if (fieldPosition>=80){
                penaltyFactor = 0.5;
            }

            if (fieldPosition+amount >= 100){
                amount = (100-fieldPosition)/2;
            }

            fieldPosition+=amount;
            if (fieldPosition>= targetPosition){
                down = 1;
            }

        }

        string po = "Offense "+oMessages[penalty];
        if (side == false){
            po = "Defense "+dMessages[penalty];
        }
        put("FLAG "+po+", "+to_string(amount)+" yds.");

    }

    void play(){
        //penalty flags
        // team homeTeam = teams[0]; //dont worry about this right now
        int flagProbability = randomInt(1,100);
        if (flagProbability<=4){
            int pickFlag = randomInt(0,sizeof(offensivePenalties)/sizeof(int)-1); //assuming both have same len

            team offendingTeam = defense;
            if (flagProbability <= 2){
                offendingTeam = offense;
                penalize(true, pickFlag);

                return;
            }

            penalize(false, pickFlag);

            return;
        }

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
            int hailMaryChance = randomInt(1,12, offense);
            int tdHailMaryChance = randomInt(1,2);

            if (hailMaryChance == 12 ){
                if (tdHailMaryChance == 2){
                    put("HAIL MARY touchdown");
                    touchdown(offense);
                    time -= 3;
                }else{
                    put("Incomplete HAIL MARY pass.");
                    time -= 2;
                    adjustFieldPosition(0);
                }
            } else {
                put("Incomplete HAIL MARY pass.");
                time -= 2;
                adjustFieldPosition(0);
            }

        } else if (down != 4 || (down==4 && targetPosition-fieldPosition<= 2 && fieldPosition>52)) {
            int playTypeChance = randomInt(1,10);

            if (getTeamScore(offense) > getTeamScore(defense) && time/down <= 3 && fieldPosition >= 10 ){ //victory foemation
                adjustFieldPosition(-2);
                time -= 2;
                put(offense.name+" sit on it");

            }else if (playTypeChance<=4 || fieldPosition>=98) { //running play
                int fumbleChance = randomInt(1, 100, offense); //<=2 for fumble
                int rand = randomInt(-2, 11, defense); //field gain
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

                int rand;
                //long / short pass
                int playChoice = randomInt(1,3);

                int completeStat = randomInt(0,10, offense);
                int interceptionProb = randomInt(1,100, offense); // <=2 for interception

                int toGo = targetPosition-fieldPosition;
                int compNumber = 4;
                if (fieldPosition < 90 && toGo>15 || playChoice == 1 || (time < 20 && getTeamScore(offense)<getTeamScore(defense) )) { // deep pass
                    interceptionProb-=(defense.dRating+1);
                    rand = randomInt(-15, 40, offense)-5; //pass length
                } else if (playChoice == 2) { // short pass
                    rand = randomInt(-10, 12, offense);
                } else if (true || playChoice == 3){ // pa pass
                    rand = randomInt(-12, 16, offense);
                }





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

                if (completeStat<=compNumber){//incomplete
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

#include <fstream>
#include <bits/stdc++.h>

void adv_tokenizer(string s, char del)
{
    stringstream ss(s);
    string word;
    while (!ss.eof()) {
        getline(ss, word, del);
        cout << word << endl;
    }
}

int main() {
    //why does a lower rating result in better stats sometimes?


    /*string t;
    ifstream Settings("settings");

    int lines = 0;
    while (getline (Settings, t)) {
        cout << "l";
        lines++;
    }

    string lns[lines];
    int i = 0;

    cout<<"BEFORE";
    while (getline (Settings,t)){
        cout << t;
        cout <<"HI"<<endl;
        adv_tokenizer(t, '=');
        lns[i] =  t;
        i++;
    }
    cout << "AFTER";

    cout<<lns[2];

    Settings.close();*/


    team bills;
    bills.oRating = 5;
    bills.dRating = 3.8;
    bills.name = "Chiefs";
    //bills designation is default team 1

    team eagles;
    eagles.oRating = 4;
    eagles.dRating = 3;
    eagles.name = "Chargers";
    eagles.designation = 2;

    double tmult = 0;
    int gamesToSimulate = 100;

    int team1Points = 0;
    int team2Points = 0;

    int team1Highest = 0;
    int team2Highest = 0;

    int team1Wins = 0;
    int team2Wins = 0;

    int ties = 0;

    time_t startSeconds;
    startSeconds = time(NULL);



    for (int ga = 1; ga <= gamesToSimulate; ga++) {
        cout << "--> Game " << ga << endl;

        time_t seconds;
        seconds = time(NULL);
        srand(seconds);

        fb game; //GAME SETTINGS AND CONSTRUCTION:
        game.teams[0] = bills;
        game.teams[1] = eagles;
        game.waitMultiplier = tmult; //When output is false and wait is 0, problems occur
        game.output = true;

        for (int i = 1; i <= 2; i++) {
            game.time = 175; //time per half
            game.half = i;
            game.doHalf(false);
            game.put("End of half " + to_string(i) + ".");

        }
        if (game.score[0] == game.score[1]) {
            game.put("Tie, end of regulation. Overtime. ");
            game.time = 100;
            game.doHalf(true);
            game.put("End of overtime.");

        }

        team1Points+= game.score[0];
        team2Points += game.score[1];

        if (game.score[0] != game.score[1]) {
            if (game.score[0] > game.score[1]) {
                team1Wins++;
            } else { team2Wins++; }
        } else {
            ties++;
        }

        if (game.score[0] > team1Highest){
            team1Highest = game.score[0];
        }
        if (game.score[1] > team2Highest){
            team2Highest = game.score[1];
        }

    cout << game.teams[0].name + ": " + to_string(game.score[0]) + ",  " + game.teams[1].name + ": " +
            to_string(game.score[1]) <<" | "<< team1Wins<<"-"<<team2Wins << endl;
    }

    time_t endSeconds;
    endSeconds = time(NULL);

    cout << endl << "~~~~~Overall Stats~~~~~" << endl;
    cout << "Took " << endSeconds-startSeconds << " seconds to simulate "<< gamesToSimulate << " games." << endl;
    cout << bills.name << " pts: "<< team1Points << ", wins: "<< team1Wins << ", high pts:" << team1Highest << ", total bias: "<< team1AddedBias << endl;
    cout << eagles.name << " pts: "<< team2Points << ", wins: "<< team2Wins << ", high pts:" << team2Highest << ", total bias: " << team2AddedBias << endl;
    cout << "ties: " << ties << endl;
    cout << "~~~~~~~~~~~~~~~~~~~~~~";

    return 0;
}
