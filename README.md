Instructions to follow to run program:

Step One: enter terminal command 'make simulation'

Step Two for Problem A:

    Run Terminal Command './simulation probA'

Step Two for Problem D:
    
    Run Terminal Command './simulation probD'

Both programs will exit after running for ten seconds.

To run with valgrind:

    valgrind --track-origins=yes --leak-check=full  --memcheck:show-reachable=yes --show-leak-kinds=all ./simulation probD -s -g

    OR 

    valgrind --track-origins=yes --leak-check=full  --memcheck:show-reachable=yes --show-leak-kinds=all ./simulation probD -s -g

IF VALGRIND FAILS PLEASE RUN AGAIN, THE THREADS CAN
TAKE TOO LONG. IT WORKS BUT TAKES AGES. PLEASE CANCEL AND RESTART. 
 
