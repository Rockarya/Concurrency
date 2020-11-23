I have implemented bonus.

MY OBSERVATIONS FOR Q1 ARE:-

1) I noticed that normal_mergeSort is always faster than both concurrent_mergeSort and threaded_mergeSort

2) Also what trend of SPEEDS i observed for n>=5 is:  normal_mergeSort >> threaded_mergeSort > concurrent_mergeSort 
	(both threaded_mergeSort and concurrent_mergeSort took same order of time) 

3) For n<5 normal_mergeSort and concurrent_mergeSort took almost same time whereas threaded_mergeSort took considerably large time.

4) At every stage where we have n<5 i have implemented selection sort as stated.

