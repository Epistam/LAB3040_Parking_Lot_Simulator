# LAB3040 
## Parking lot simulator

### Description
This project is a 1st year (bac+3) C project supposed to emulate a parking lot. 
It features dynamic layouts defined in files and automatic car behavior, along
with various options such as simulation speed, probabilistic accidents, and
lots of other things. 


### Technical choices
carossserie size = consts 

use center of car so that calculation doesn't depend on orientation

1 temps pour tourner


car id


quand on parcourt la liste pour ajouter une voiture, on prend un unsigned int id supérieur à tout le reste mais < à INT_MAX sinon on exit le programme (devrait pas y avoir d'overflow sur ça ptn)



### The rendering method 

The game relies on a main "map", and array that represents every element in the game in an orthonormal fashion (unlike the terminal). 
For every iteration, a "new map" is updated, and the program parses both maps and adds every difference between those in a queue so that only the modified parts are changed on the actual display. 

Then the map is displayed, double characters > means the doubling up needs to be treated very carefuly and in a general way for both the map and the cars

Cars are a specific matter, since the "doubling" needs to be done in a certain way. Thus we use a placeholder character on the map for cell used by a car, and mark the center of the car with a specific character, which will be used to call a routine displaying the car specifically.



status bar struct : pass info to it and refresh its display
