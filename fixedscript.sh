# command line arguments are in the form ./bst --hash-workers n 

#write a script that runs the above command with n from 1 to 10000 incrementing by 2 each time and averaging the value over 5 runs
#output the console output to a file called parallelhashtime.txt

#run the script with the command ./parallelhash.sh



#!/bin/bash
make compile
# Specify the output file
o_lock_stack="lock_stack.txt"
o_free_stack="free_stack.txt"
o_lock_queue="lock_queue.txt"
o_free_queue="free_queue.txt"
o_lock_map="lock_map.txt"
o_free_map="free_map.txt"

echo "" > "lock_stack.txt"
echo "" > "free_stack.txt"
echo "" > "lock_queue.txt"
echo "" > "free_queue.txt"
echo "" > "lock_map.txt"
echo "" > "free_map.txt"



# Loop from 1 to 10000, incrementing by 2
# for n in {1000..1000000000}; do
for (( n=1000; i <= 1000000000; i *= 10 ))
do
    total_time_lock_stack=0
    total_time_free_stack=0
    total_time_lock_queue=0
    total_time_free_queue=0
    total_time_lock_map=0
    total_time_free_map=0
    


    # Run the command 5 times and average the results
    for run in {1..5}
    do
        # Run the command and capture the time output
        command_output=$(./main -r=4 -w=4 -R=$n -W=$n >&1)
        # /bst --hash-workers 5  --data-workers 10 --input fine.txt --comp-workers -2 > outpasdf.txt

        # Extract the time value from the command output (modify this part based on your actual command output)
        lock_stack=$(echo "$command_output" | grep -oE 'LockStack: [0-9]+\.[0-9]+')
        free_stack=$(echo "$command_output" | grep -oE 'FreeStack: [0-9]+\.[0-9]+')
        lock_queue=$(echo "$command_output" | grep -oE 'LockQueue: [0-9]+\.[0-9]+')
        free_queue=$(echo "$command_output" | grep -oE 'FreeQueue: [0-9]+\.[0-9]+')
        lock_map=$(echo "$command_output" | grep -oE 'LockMap: [0-9]+\.[0-9]+')
        free_map=$(echo "$command_output" | grep -oE 'FreeMap: [0-9]+\.[0-9]+')

        # Check if time_value is not empty
        if [ -n "$lock_stack" ]; then
            total_time_lock_stack=$(awk "BEGIN { print $total_time_lock_stack + ${lock_stack#LockStack: } }")
        if [ -n "$free_stack" ]; then
            total_time_free_stack=$(awk "BEGIN { print $total_time_lock_stack + ${lock_stack#FreeStack: } }")
        if [ -n "$lock_queue" ]; then
            total_time_lock_queue=$(awk "BEGIN { print $total_time_lock_stack + ${lock_stack#LockQueue: } }")
        if [ -n "$free_queue" ]; then
            total_time_free_queue=$(awk "BEGIN { print $total_time_lock_stack + ${lock_stack#FreeQueue: } }")
        if [ -n "$lock_map" ]; then
            total_time_lock_map=$(awk "BEGIN { print $total_time_lock_stack + ${lock_stack#LockMap: } }")
        if [ -n "$free_map" ]; then
            total_time_free_map=$(awk "BEGIN { print $total_time_lock_stack + ${lock_stack#FreeMap: } }")
        else
            echo "Error extracting time value for n=$n, run $run"
        fi
    done

    # Calculate the average time over 5 runs
    average_time_lock_stack=$(awk "BEGIN { print $total_time_lock_stack / 5 }")
    average_time_free_stack=$(awk "BEGIN { print $total_time_free_stack / 5 }")
    average_time_lock_queue=$(awk "BEGIN { print $total_time_lock_queue / 5 }")
    average_time_free_queue=$(awk "BEGIN { print $total_time_free_queue / 5 }")
    average_time_free_map=$(awk "BEGIN { print $total_time_free_map / 5 }")
    average_time_lock_map=$(awk "BEGIN { print $total_time_lock_map / 5 }")

    # Write the result to the output file
    echo "$average_time_lock_stack" >> "$o_lock_stack"
    echo "$average_time_free_stack" >> "$o_free_stack"
    echo "$average_time_lock_queue" >> "$o_lock_queue"
    echo "$average_time_free_queue" >> "$o_free_queue"
    echo "$average_time_lock_map" >> "$o_lock_map"
    echo "$average_time_free_map" >> "$o_free_map"

done
