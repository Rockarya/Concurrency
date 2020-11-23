FEATURES:-

1)Used appropriate small delays for the simulation.
2)Used only one mutex lock per Pharmaceutical Company, Vaccination Zone.
3)Used threads for concurrent vaccination process,concurrent student arrival and concurent vaccine production.
4)Used busy waiting wherever i find necessary.
5)Used rand() function to randomize.

LOGIC:-
->Firstly created n Pharmaceutical Company threads which will run concurrently and produce vaccines.
->After vaccines are generated then they will be send to the Vaccinating zones which will allot slots to the students to get vaccinated.Here also m threads for m vaccinating zones.(They will also run concurrently)
->Arrival of students is also kept concurrent but with random delays b/w [0,5] seconds.So some students may arrive at same time and same may be at different times. 
->Added comments in code for more clarity.
