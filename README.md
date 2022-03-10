STARLINK BEAM PLANNING TECH TEST:

This is Aditya Shekar's Technical Submission for the New Grad SpaceX Starlink Role.

An automated script has been provided that runs all of the tests provided in test_cases.

Simply compile the source code:
g++ optimize.cpp

and ensure that the executable is named a.out

Then run:
./automated.sh

Further questions about how to compile or test the program can be asked at 

adityajittoo@gmail.com (Primary)
ashekar1@alumni.cmu.edu
2085703101

The code performs well on all test cases and satisfies all constraints. It optimizes user beams based on access and prior assignments.
All simple cases are completely correct, and larger ones range between 70-95% correctness.
Many of the causes of lower performance include constraints on how users can access satellites and how many beams a single satellite may have.
Test 11 passes roughly 30% of cases, and while at face value this may seem low, further investigation suggests that if 1440 satellites can each serve a 
maximum of 32 users, then in total we can only ever hope to provide connections to 46k unique users (46%).
Upon further constraining of the problem (including non-starlink satellites, angle of access, and beam color), it appears as though 30% might be significantly better than expected!



