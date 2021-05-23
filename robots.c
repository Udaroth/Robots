#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#define BOARD_SIZE 8
#define BUF_SIZE 100
#define ROBOT_ARRAY_SIZE 10
#define DIRECTION_SIZE 2
#define INT_OFFSET 48

struct Human {

    int x;
    int y;
    char axisOfPreviousMove;

};

struct Robot {

    int x;
    int y;
    bool alive;

};

struct Exit {

    int x;
    int y;

};

struct Collision {
    int x;
    int y;
    bool occupied;
};

// Delcare some fuctions
void beginGame();

void checkWinCondition();

char map[BOARD_SIZE][BOARD_SIZE];

int numOfRobots = 0;

struct Human human;

struct Robot robots[ROBOT_ARRAY_SIZE] = {0};

struct Collision collisions[ROBOT_ARRAY_SIZE/2] = {0};

struct Exit exitLocation;

bool gameFinished = false;

bool firstGame = true;




void setUp(){

    char buf[BUF_SIZE];

    bool humanCoordinates = false;

    while(!humanCoordinates){

        // MARK: Human set up
        printf("Human starting point: ");

        fgets(buf, sizeof(buf), stdin);

        // printf("Human stored at :%s\n", buf);

        int x = buf[0] - 48;
        int y = buf[2] - 48;

        if(x > 7 || x < 0 || y < 0 || y > 7){

            printf("Invalid Coordinates, Try again\n");
            continue;
        } else {
            // Valid coordinates
            human.x = x;
            human.y = y;
            human.axisOfPreviousMove = 'N';
            humanCoordinates = true;
        }

    }

    // MARK: Set number of robots (I thought it was meant to be random but whatever)
    
    bool robotNumbers = false;

    while(!robotNumbers){

        printf("Number of robots: ");

        fgets(buf, sizeof(buf), stdin);
        
        numOfRobots = buf[0] - INT_OFFSET;

        if(numOfRobots < 1 || numOfRobots > 5){
            printf("Invalid number, Try again\n");
            continue;
        } else {
            
            // Set that number of robots to be alive

            for(int i = 0; i < ROBOT_ARRAY_SIZE; i++){

                if(i < numOfRobots){
                    robots[i].alive = true;
                } else {
                    robots[i].alive = false;
                }

            }

            robotNumbers = true;

        }

    }

    // MARK: Setting up Robot Coordinates

    int i = 0;

    while(i < numOfRobots){

        if(robots[i].alive == true){

            printf("Robot %d: ", i + 1);

            fgets(buf, sizeof(buf), stdin);

            // printf("%s\n", buf);

            int x = buf[0] - INT_OFFSET;
            int y = buf[2] - INT_OFFSET;

            // Perform Checks on the coordinates before storing them
            if(x < 0 || y < 0 || x  > 7 || y > 7){

                printf("Invalid Coordinates, Try again\n");
                // Prompt again without incrementing robots
                continue;

            } else if (abs(human.x-x)+abs(human.y-y) < 3){
                // Perform checks on whether it is too close to human
                printf("Invalid Coordinates, Try again\n");
                continue;
                
            } else {

                bool coordClash = false;

                for(int j = 0; j < numOfRobots; j++){
                    if(robots[j].x == x && robots[j].y == y){
                        // Another robot already have the same coordinate
                        printf("Invalid Coordinates, Try again\n");
                        coordClash = true;
                    }
                }

                if(coordClash){
                    continue;
                }

                // Store the coordinates of each robot into the array of robot structs
                robots[i].x = x;
                robots[i].y = y;
                i++;

            }



        }


    }


    // MARK: Randomizing Exit
    exitLocation.x = rand() % 8;
    exitLocation.y = rand() % 8;


    //MARK: Place Human, Robots, and Exits onto the map
    // Clear map
    for(int m = 0; m < BOARD_SIZE; m++){
        for(int n = 0; n < BOARD_SIZE; n++){
            map[m][n] = '0';
        }
    }
    
    // Human
    map[human.y][human.x] = 'H';

    // Robots
    for(int t = 0; t < numOfRobots; t++){
        map[robots[t].y][robots[t].x] = 'R';
    }

    // Exit
    map[exitLocation.y][exitLocation.x] = 'E';

    // Initialise collisions
    for(int p = 0; p < ROBOT_ARRAY_SIZE/2; p++){
        collisions[p].occupied = false;
    }

    if(firstGame){
        printf("\n");
    }

    
    beginGame();

}

void checkLoseCondition(){

    // Check if any of the robots coordinates equal to the human coordinate
    for(int i = 0; i < numOfRobots; i++){
        if(robots[i].x == human.x && robots[i].y == human.y){
            // Caught the human
            printf("\nThe robots got you! We are doomed!\n\n");
            firstGame = false;
            gameFinished = true;
            setUp();
            break;
        }
    }

}

void updateMap(){

    // Clear Map
    for(int i = 0; i < BOARD_SIZE; i++){
        for(int j = 0; j< BOARD_SIZE; j++){
            map[i][j] = '0';
        }
    }

    // Human
    map[human.y][human.x] = 'H';

    // Robots
    for(int t = 0; t < numOfRobots; t++){
        map[robots[t].y][robots[t].x] = 'R';
    }

    // Exit
    map[exitLocation.y][exitLocation.x] = 'E';

    // Collisions
    for(int p = 0; p < ROBOT_ARRAY_SIZE/2; p++){
        if(collisions[p].occupied){
            map[collisions[p].y][collisions[p].x] = 'D';
        }
    }


    

}

void updateRobotCoordinates(){
        // Update Robot Coordinates
    for(int k = 0; k < numOfRobots; k++){
        // Check which axis the robot is further from the human on

        if(abs(robots[k].x - human.x) > abs(robots[k].y - human.y)){
            // Difference larger on the x axis
            // Move robot towards human on this axis
            int robotMove = 0;
            robotMove = (human.x - robots[k].x)/abs(human.x - robots[k].x);
            // Robot's new position given by
            robots[k].x += robotMove;
        } else if (abs(robots[k].x - human.x) < abs(robots[k].y - human.y)){
            // Difference larger on the y axis
            // Move robot towards human on this axis
            int robotMove = 0;
            robotMove = (human.y - robots[k].y)/abs(human.y - robots[k].y);
            // Robot's new position given by
            robots[k].y += robotMove;
        } else {
            // Equally far away on the x and y axis
            // Move according to human's previous move
            if(human.axisOfPreviousMove == 'X'){
                // Move on Y axis
                int robotMove = 0;
                robotMove = (human.y - robots[k].y)/abs(human.y - robots[k].y);
                // Robot's new position given by
                robots[k].y += robotMove;
            } else if (human.axisOfPreviousMove == 'Y'){
                // Move on X axis
                int robotMove = 0;
                robotMove = (human.x - robots[k].x)/abs(human.x - robots[k].x);
                // Robot's new position given by
                robots[k].x += robotMove;
            }

        }


    }

    // Check if any robots collided
    for(int j = 0; j < numOfRobots; j++){
        for(int k = 0; k < numOfRobots; k++){
            if(j != k){
                // Only perform this operation on different robots
                if(robots[j].x == robots[k].x && robots[j].y == robots[k].y){
                    // Two robots with same coordinates
                    // Collision has occured
                    // Set both robots to !alive
                    robots[j].alive = false;
                    robots[k].alive = false;
                    // Store collision location into collision array
                    for(int i = 0; i < ROBOT_ARRAY_SIZE/2; i++){
                        if(!collisions[i].occupied){
                            // Store it here because its still empty
                            collisions[i].x = robots[j].x;
                            collisions[i].x = robots[j].y;
                            collisions[i].occupied = true;
                            break;
                        }
                    }
                }
            }
        }
    }
}



void handleMove(char direction[DIRECTION_SIZE]){

    // printf("Direction Parsed %s\n", direction);
    int east = strncmp(direction, "ET", 2); 
    int west = strncmp(direction, "WT", 2); 
    int south = strncmp(direction, "ST", 2); 
    int north = strncmp(direction, "NT", 2); 


    if(east == 0){
        // printf("East\n");

        // Move human first 
        // Moving East means one cell to the right
        // Check the new cell is not out of bounds
        if(human.x + 1 >= BOARD_SIZE){
            // Out of bounds
            // Do nothing I suppose
            printf("Invalid Move\n");
            return;
        }

        // Update Human Coordinate and moveAxis
        human.x += 1;
        human.axisOfPreviousMove = 'X';

        // Check if Human has won
        checkWinCondition();
        
        // Update robot coordinates
        updateRobotCoordinates();


    } else if (west == 0) {
        // printf("West\n");
        if(human.x - 1 < 0){
            // Out of bounds
            // Do nothing I suppose
            printf("Invalid Move\n");
            return;
        }
        // Update Human Coordinate and moveAxis
        human.x -= 1;
        human.axisOfPreviousMove = 'X';

        // Check if Human has won
        checkWinCondition();

        // Update robot coordinates
        updateRobotCoordinates();



    } else if (north == 0) {

        if(human.y - 1 < 0){
            // Out of bounds
            // Do nothing I suppose
            printf("Invalid Move\n");
            return;
        }
        // Update Human Coordinate and moveAxis
        human.y -= 1;
        human.axisOfPreviousMove = 'Y';

        // Check if Human has won
        checkWinCondition();

        // Update robot coordinates
        updateRobotCoordinates();

    } else if (south == 0) {

        if(human.y + 1 >= BOARD_SIZE){
            // Out of bounds
            // Do nothing I suppose
            printf("Invalid Move\n");
            return;
        }
        // Update Human Coordinate and moveAxis
        human.y += 1;
        human.axisOfPreviousMove = 'Y';

        // Check if Human has won
        checkWinCondition();

        // Update robot coordinates
        updateRobotCoordinates();

    }

    // After all the robots have been moved 

    // Check if human has been caught
    checkLoseCondition();

    // Update map for display
    updateMap();

    printf("\n");

    return;
}

void checkWinCondition(){

    if(human.x == exitLocation.x && human.y == exitLocation.y){
        // Human has won
        printf("\nWell done! You survived, but it isn't over!\n");
        firstGame = false;
        gameFinished = true;
        // Call setup here again
        printf("\n\n");
        setUp();
    }

    //MARK: Add checking if all robots are dead
    bool robotsAlive = false;
    for(int i = 0; i < ROBOT_ARRAY_SIZE;i++){
        if(robots[i].alive == true){
            robotsAlive = true;
            break;
        }
    }

    if(robotsAlive == false){
        printf("\nWell done! You survived, but it isn't over!\n");
        firstGame = false;
        gameFinished = true;
        // Call set up again
        printf("\n\n");
        setUp();
    }

}


void handleDisplay(){


    // Assume map is updated so just print the map as it is    
    printf("\n");

    for(int y = 0; y < BOARD_SIZE; y++){
        printf("|");
        for(int x = 0; x < BOARD_SIZE; x++){
            
            if(map[y][x] == 'H'){
                printf("H");
            } else if (map[y][x] == 'E'){
                printf("E");
            } else if (map[y][x] == 'R'){
                printf("R");
            } else if (map[y][x] == 'D'){
                printf("D");
            } else {
                printf(" ");
            }

            printf("|");

        }


        // End of each row
        printf("\n");
    }

    printf("\n");

    return;
}


void beginGame(){

    char buf[BUF_SIZE];

    gameFinished = false;

    bool startOfGame = true;

    while(fgets(buf, sizeof(buf), stdin) && !gameFinished){
        


        int move = strncmp(buf, "move", 4);    
        int display = strncmp(buf, "display", 6);
        int exit = strncmp(buf, "exit", 4);


        // printf("move = %d, display = %d, exit = %d\n", move, display, exit);
        // printf("\n");

        if(exit == 0){
            // printf("Exit called\n");
            gameFinished = true;
            return;
        } else if (move == 0){

            // printf("\n");

            char direction[DIRECTION_SIZE];

            strncpy(direction, &buf[5], DIRECTION_SIZE*sizeof(char));

            handleMove(direction);


        } else if (display == 0){

            // printf("\n");
            
            handleDisplay();

        }



    }



}



int main(int argc, char* argv[]){

    srand(atoi(argv[1]));

    // printf("Seed given %s, Int form %d\n", argv[1], atoi((argv[1])));
    // printf("%d arguments %s\n", argc, argv[1]);

    setUp();


}
