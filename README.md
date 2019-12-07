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
