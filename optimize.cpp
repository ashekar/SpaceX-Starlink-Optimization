#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>
#include <utility>

#define PI 3.14159265
#define COLORINDEX 2
#define OVERFLOWSATS 33

/*
Sorting Function for Vectors
*/
struct sorter{
    bool operator()(const std::pair<int, int> &left, const std::pair<int, int> &right){
        return left.second < right.second;
    }
};

/*
Function to construct opposite side of vectors.
*/
std::vector<double> ConstructVector(std::vector<double> tip, std::vector<double> base){
    double x3 = tip[0] - base[0];
    double y3 = tip[1] - base[1];
    double z3 = tip[2] - base[2];
    std::vector<double> newArrow;
    newArrow.push_back(x3);
    newArrow.push_back(y3);
    newArrow.push_back(z3);
    return newArrow;
}

/*
Test if the angle between two vectors is within range 
*/
bool WithinAngleRange(std::vector<double> a, std::vector<double> b, double range){
    double dotproduct = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    double aHypotenuse = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    double bHypotenuse = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
    double quotient = dotproduct/(aHypotenuse * bHypotenuse);
    double degrees = acos(quotient) * 180/PI;
    if(degrees < range){
        return true;
    }
    return false;

}


/*
Build Map of which users can access which satellites.
*/
void BuildInRangeSatellitesDictionary(std::map<int, std::vector<double> > &users, 
                                        std::map<int, std::vector<double> > &sats,
                                        std::map<int, std::vector<int> > &inRangeSats){
    
    std::map<int, std::vector<double> >::iterator currentUser;
    for(currentUser = users.begin(); currentUser != users.end(); ++currentUser){
        int userID = currentUser -> first;
        std::vector<double> userCoordinates = currentUser -> second;
        std::vector<int> satelliteIDS;

        std::map<int, std::vector<double> >::iterator currentSat;
        for(currentSat = sats.begin(); currentSat != sats.end(); ++currentSat){
            int satID = currentSat -> first;
            std::vector<double> satCoordinates = currentSat -> second;

            std::vector<double> constructedVector = ConstructVector(satCoordinates, userCoordinates);
            if(WithinAngleRange(constructedVector, userCoordinates, 45.00)){
                satelliteIDS.push_back(satID);
            }
        }

        inRangeSats[userID] = satelliteIDS;

    }          
}


/*
Filter the the list of in range satellites with interferers.
*/
void BuildValidSatellitesDictionary(std::map<int, std::vector<double> > &users, 
                                    std::map<int, std::vector<double> > &sats,
                                    std::map<int, std::vector<double> > &interferers,
                                    std::map<int, std::vector<int> > &inRangeUserSatsMap,
                                    std::map<int, std::vector<int> > &validUserSatsMap){

    std::map<int, std::vector<int> >::iterator currentUser;
    for(currentUser = inRangeUserSatsMap.begin(); currentUser != inRangeUserSatsMap.end(); ++currentUser){
        int userID = currentUser -> first;
        std::vector<double> userCoordinates = users[userID];
        std::vector<int> inRangeSatellites = currentUser -> second;

        std::vector<int> validSatellites;
        for(int index = 0; index < inRangeSatellites.size(); index++){
            int satID = inRangeSatellites[index];
            std::vector<double> satCoordinates = sats[satID];

            std::vector<double> constructedVectorOne = ConstructVector(satCoordinates, userCoordinates);
            std::map<int, std::vector<double> >::iterator currentInterferer;
            bool validSatIndicator = true;
            for(currentInterferer = interferers.begin(); currentInterferer != interferers.end(); ++currentInterferer){
                std::vector<double> interCoordinates = currentInterferer -> second;
                std::vector<double> constructedVectorTwo = ConstructVector(interCoordinates, userCoordinates);
                if(WithinAngleRange(constructedVectorOne, constructedVectorTwo, 20.00)){
                    validSatIndicator = false;
                }
            }
            if(validSatIndicator){
                validSatellites.push_back(satID);
            }
        }
        validUserSatsMap[userID] = validSatellites;

    }
}


/*
Map Satellites to Filtered Users (Flip output of previous function)
*/
void CreateSatellitesToUsers(std::map<int, std::vector<int> > &validUserSatsMap,
                             std::map<int, std::vector<int> > &validUsersForSatellites){

    std::map<int, std::vector<int> >::iterator currentUser;
    for(currentUser = validUserSatsMap.begin(); currentUser != validUserSatsMap.end(); ++currentUser){
        int userID = currentUser -> first;
        std::vector<int> satellites = currentUser -> second;

        for(int i = 0; i < satellites.size(); i++){
            int satID = satellites[i];
            if(validUsersForSatellites.find(satID) != validUsersForSatellites.end()){
                (validUsersForSatellites[satID]).push_back(userID);
            }else{
                std::vector<int> usersToAdd;
                usersToAdd.push_back(userID);
                validUsersForSatellites[satID] = usersToAdd;
            }
        } 
    }
}


/*
Find which users are within a 10 degree angle of the same starlink satellite
*/
void CreateColorConflicts(std::map<int, std::vector<double> > &users, 
                          std::map<int, std::vector<double> > &sats,
                          std::vector<int> &freshUserIDS,
                          std::map<int, std::vector<int> > &userColorConflicts,
                          int satID){

    for(int i = 0; i < freshUserIDS.size(); i++){
        int firstUserID = freshUserIDS[i];
        std::vector<int> conflictingIDS;

        for(int j = 0; j < freshUserIDS.size(); j++){
            int secondUserID = freshUserIDS[j];
            if(i != j){
                std::vector<double> satelliteCoordinates = sats[satID];
                std::vector<double> userOneCoordinates = users[firstUserID];
                std::vector<double> userTwoCoordinates = users[secondUserID];

                std::vector<double> ConstructedVectorOne = ConstructVector(userOneCoordinates, satelliteCoordinates);
                std::vector<double> ConstructedVectorTwo = ConstructVector(userTwoCoordinates, satelliteCoordinates);

                if(WithinAngleRange(ConstructedVectorOne, ConstructedVectorTwo, 10.00)){
                    conflictingIDS.push_back(secondUserID);
                }
            }
        }
        userColorConflicts[firstUserID] = conflictingIDS;
    }

}


/*
Map numbers to colors.
*/
std::string matchColorNumberToLabel(int colorToConsider){
    if(colorToConsider == 1){
        return "A";
    }else if(colorToConsider == 2){
        return "B";
    }else if(colorToConsider == 3){
        return "C";
    }else if(colorToConsider == 4){
        return "D";
    }
    return "ERROR";
}

/*
Optimize assignments of users to StarLink satellites
*/
void OptimizeMatches(std::map<int, std::vector<double> > &users, 
                     std::map<int, std::vector<double> > &sats,
                     std::map<int, std::vector<int> > &validUsersForSatellites, 
                     std::map<int, std::vector<int> > &userMatches,
                     std::vector<std::pair<int, int> > &orderedByUserSatellites){

    std::vector<std::pair<int, int> >::iterator currentPairIter;
    for(currentPairIter = orderedByUserSatellites.begin(); currentPairIter != orderedByUserSatellites.end(); ++currentPairIter){ 
        std::pair<int, int> currentPair = *currentPairIter;
        int satID = currentPair.first;
        
        int beamNumber = 1;
        std::vector<int> userIDS = validUsersForSatellites[satID];
        std::vector<int> freshUserIDS;

        for(int i = 0; i < userIDS.size(); i++){
            int iterID = userIDS[i];
            if(userMatches.find(iterID) == userMatches.end()){
                
                freshUserIDS.push_back(iterID);
            }
        }

        std::map<int, std::vector<int> > userColorConflicts;
        CreateColorConflicts(users, sats, freshUserIDS, userColorConflicts, satID);
        

        std::set<int> allColors;
        allColors.insert(1);
        allColors.insert(2);
        allColors.insert(3);
        allColors.insert(4);

        for(int z = 0; z < freshUserIDS.size(); z++){
            int currentEvaluateID = freshUserIDS[z];
            if(beamNumber == OVERFLOWSATS){
                break;
            }if(userMatches.find(currentEvaluateID) != userMatches.end()){
                continue;
            }
            
            std::vector<int> problemIDS = userColorConflicts[currentEvaluateID];
            std::set<int> impossColors;
            for(int y = 0; y < problemIDS.size(); y++){
                int individualProblemID = problemIDS[y];
                
                if(userMatches.find(individualProblemID) != userMatches.end()){
                    std::vector<int> assignment = userMatches[individualProblemID];
                    impossColors.insert(assignment[COLORINDEX]);
                }
            }

            std::set<int>::iterator chooseColorIterator;
            for(chooseColorIterator = allColors.begin(); chooseColorIterator != allColors.end(); chooseColorIterator++){
                int colorToConsider = *chooseColorIterator;
                
                if(impossColors.find(colorToConsider) == impossColors.end()){
                    std::vector<int> configuration;
                    configuration.push_back(satID);
                    configuration.push_back(beamNumber);
                    configuration.push_back(colorToConsider);

                    std::string colorLabel = matchColorNumberToLabel(colorToConsider);
                    std::cout << "sat " << satID << " beam " << beamNumber << " user " << currentEvaluateID << " color " << colorLabel << "\n";
                    userMatches[currentEvaluateID] = configuration;
                    beamNumber++;
                    break;
                }
            }


        }


    }

}



/*
Parse Input
*/
void PopulateDictionaries(char *filename, 
                          std::map<int, std::vector<double> > &users, 
                          std::map<int, std::vector<double> > &sats,
                          std::map<int, std::vector<double> > &interferers){
    
    std::string line;
    std::ifstream myfile;
    myfile.open(filename);

    if(!myfile.is_open()){
        perror("Error Open");
        exit(EXIT_FAILURE);
    }

    while(getline(myfile, line)){
        if(line.length() == 0){
            continue;
        }
        std::vector<std::string> tokens;
        std::stringstream check1(line);
        std::string iteratorstr;
        int flagbit = -1;
        bool hashtag = false;

        while(getline(check1, iteratorstr, ' ')){
            
            if(iteratorstr == "#"){
                hashtag = true;
                break;
            }
            if(iteratorstr == "user"){
                flagbit = 0;
            }
            else if(iteratorstr == "sat"){
                flagbit = 1;
            }
            else if(iteratorstr == "interferer"){
                flagbit = 2;
            }
            else{
                tokens.push_back(iteratorstr);
            }
        }

        if(!hashtag){
            std::vector<double> nums;
            int id = std::stoi(tokens[0]);
            for(int i = 1; i < tokens.size(); i++){
                std::string currentDouble = tokens[i];
                double num = std::stod(currentDouble);
                nums.push_back(num);
            }
            
            
            switch(flagbit){
                case 0:
                    
                    users[id] = nums;
                    break;
                case 1:
                    sats[id] = nums;
                    break;
                case 2:
                    interferers[id] = nums;
                    break;
                default:
                    printf("Error with input format\n");
            }
        }
        
    }                        
}

/*
Sort Satellites based on how many users they serve.
*/
void CreateSortedVectorForSatellites(std::map<int, std::vector<int> > &validUsersForSatellites, 
                                     std::vector<std::pair<int, int> > &orderedByUserSatellites){
    std::map<int, std::vector<int> >::iterator iter;
    
    for(iter = validUsersForSatellites.begin(); iter != validUsersForSatellites.end(); ++iter){
        int ID = iter -> first;
        std::vector<int> users = iter -> second;
        
        std::pair<int, int> insertValue = std::make_pair(ID, users.size());
        orderedByUserSatellites.push_back(insertValue);
    }

    std::sort(orderedByUserSatellites.begin(), orderedByUserSatellites.end(), sorter());
    
}



/*
Main Function
*/
int main(int argc, char** argv){
    char* filename = argv[1];
    std::map<int, std::vector<double> > users, sats, interferers;
    std::map<int, std::vector<int> > inRangeSats, validUserSatsMap, validUsersForSatellites, userMatches;
    std::vector<std::pair<int, int> > orderedByUserSatellites;


    PopulateDictionaries(filename, users, sats, interferers);
    BuildInRangeSatellitesDictionary(users, sats, inRangeSats);
    BuildValidSatellitesDictionary(users, sats, interferers, inRangeSats, validUserSatsMap);
    CreateSatellitesToUsers(validUserSatsMap, validUsersForSatellites);
    CreateSortedVectorForSatellites(validUsersForSatellites, orderedByUserSatellites);
    OptimizeMatches(users, sats, validUsersForSatellites, userMatches, orderedByUserSatellites);

    return 0;


}
