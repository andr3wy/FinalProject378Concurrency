import re
import matplotlib.pyplot as plt

names = ["free_map", "lock_map", "free_queue", "lock_queue", "free_stack", "lock_stack"]
colors = ["red", "blue", "green", "purple", "orange", "pink"]

plt.figure(figsize=(10, 6))
plt.title("Single Writer with Scaling Readers (10 Million Operations Each)")
plt.xlabel('Number of Readers')
plt.ylabel('Time (Milliseconds)')

for i in range(len(names)) :
    nam = names[i]
    color = colors[i]
    # Read the input data from the file
    file_path = nam + ".txt"

    # file_path = "res1.txt"
    # file_path = "res2.txt"
    # file_path = "res3.txt"
    with open(file_path, 'r') as file:
        data_str = file.read()

    # Parse the input data
    data_blocks1 = re.split(r'\n', data_str.strip())  # Split into data blocks

    x_values = []
    data_blocks = []
    print(data_blocks)

    for block in data_blocks1:
        data_blocks.append(float(block))
    # Create the graph
    x_values = list(i for i in range(0, 8))


    plt.plot(x_values[0:8], data_blocks[0:8], marker='o', linestyle='-', color=color, label=nam)
    plt.legend(loc="upper left")
    plt.grid(True)

    # Save the plot to a local file
output_file_path = "nam" + ".png"
plt.savefig("out/" + output_file_path)