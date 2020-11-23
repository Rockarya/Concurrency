1)I have implemented 1st bonus.
2)Have coloured output
3)I have created threads for concurrent processes.

LOGIC PART:-
->I have 3 semaphores in my code.One for electric stage,one for acoustic stage,one for co-ordinators.
->Everytime iterating over loop i m creating a thread to accomplish it.
->To wait for arrival time i have used sleep(arrival time).
->To give performing time to a process i have used rand() function.
->If a singer perfroms then there are 3 possibilities which are :-
			->Singer performs solo on electric stage.
			->Singer performs solo on acoustic stage.
			->Singer joins some performance(implemented a timer which regularly checks for semaphore to be get free)
		I have implemented all these cases concurrently by using 3 threads for them and raising the flag to 1 if either of 
		them satisfies in the given time limit(implemented through sem_timedwait() function)
		Also if flag is raised to 1 then modifying value of patient[iterator] to be 1 so that tshirt can be granted to them.
		Otherwise singer will leave due to impatience.

->Similarly if a Musician performs then there are 2 possibilities which are :-
			->Musician goes to acoustic stage.
			->Musician goes to electric stage.
		Here also i have used thread to implement both the processes and rasing the flag to be 1 if either of them satisfies
		in the given time limit also modifying patient[iterator] to be 1 so that tshirt can be granted to them. 
		Otherwise Musician will leave due to impatience.
