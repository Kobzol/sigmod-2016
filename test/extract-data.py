import shutil
import sys

batch_size = 1000
max_lines = 1000000000000000000
batch_counter = 0
line_counter = 0
TEST_FILE = "test-data.txt"

if len(sys.argv) > 1:
    max_lines = int(sys.argv[1])
if len(sys.argv) > 2:
    batch_size = int(sys.argv[2])

shutil.rmtree(TEST_FILE, True)
shutil.copy("./init-file.txt", TEST_FILE)

with open("workload-file.txt", "r") as workload:
    with open(TEST_FILE, "a") as test_data:
        test_data.write("S\n")

        for line in workload:
            test_data.write(line)
            batch_counter += 1
            line_counter += 1

            if line_counter == max_lines:
                break

            if batch_counter == batch_size:
                test_data.write("F\n")
                batch_counter = 0
        if batch_counter != 0:
            test_data.write("F\n")
