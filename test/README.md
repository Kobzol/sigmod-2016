# ACM SIGMOD 2016 Programming Contest Test Harness

This package contains code for the test harness used to evaluate submissions.
It is provided with the intention to allow participants to thoroughly test the correctness of their submissions offline before submitting their code for evaluation.
To build the test harness (assuming a fairly recent version of `gcc` is available on the `PATH`), simply type `make harness`.

A set of sample inputs and outputs is also provided.
You can test a solution by running the following command:

    ./harness init-file.txt workload-file.txt result-file.txt SOLUTION_EXECUTABLE_PATH

This command will likely take several minutes to run and if the solution is correct, it will output the number of milliseconds required to answer all of the queries in the workload.
For solutions which are incorrect, the first 100 queries which produce incorrect values will be printed along with the expected value.
