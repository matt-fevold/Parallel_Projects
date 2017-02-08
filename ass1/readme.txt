to compile: g++ -O1 -w assignment1.cpp -lpthread -o assignment1

to run: ./assignment1 /home/mfevold/git-repos/projects/parallel/ass2/HcNetwork.txt 1 

where arg 1 is path to file and arg 2 is number of threads

Runtime of HcNetwork at various threads(on my personal laptop with 4 cores):
(in microseconds)

1: 323020
2: 257528
4: 240063
8: 165783
16: 168165
32: 170245
37 (just for fun and to show that any number of threads work): 188746

This ended up being more difficult than I gave it credit for but I am happy/proud of my code. Hopefully you will accept this late submission.

 
 


