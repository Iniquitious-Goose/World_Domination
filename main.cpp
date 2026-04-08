#include <iostream>
#include <string>
#include <cctype>
#include "game.hpp"








int main() {

srand(time(0));

Game game;

bool playing = true;

while(playing) {
    game.showIntro();

    while (!game.isOver()) {
        game.play();          
    } 

    char choice;

    std::cout << "\nPlay again? (y/n): ";
    
    switch(game.getInput()) {
        case 'y': break;
        default: 
        std::cout << "\nThanks for playing!\n";
        playing = false;
    }; 
}
    return 0;
}






/*
You are to write a game similar to the "Linked list game" in the notes. 
There should be 20 nodes. You can change the struct to your preferences 
in order to make the game better. 


Make sure the user is not able to move to a NULL location.



You can add special conditions to where the story will not progress
 unless you clear a certain obstacle in a different room, 
 others did enemy encounters where the enemy will 
 drop a key to open a door. 
 You should make your story / game interesting (
 it should also be clear to navigate.. 
 if I get lost in your program, your grade will suffer!). 
 A sort of map may help the user to navigate.


**if you look at the game tutorial I submitted, 
this homework is really not difficult at all.. 
just time consuming**
*/

