# multithreaded-search-engine-with-TCP

This project is written in cpp and simulates a search engine.

Threads are used, and protected data structures I wrote and a semaphore to ensure synchronization between threads.
Communication between the server and the user is done using tcp.

To run the project you need to run the server_build file and execute all the commands required there, and then run the client_build file.

You can search for one word (over 3 characters) or several words, the search results will be the 10 sites with the most appearances of these words.
In addition, you can search for pages where a certain word is not found, it is done like this:
dog -cat
So the search results will be all pages that have dog but no cat.
