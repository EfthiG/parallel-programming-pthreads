import os
import numpy as np
import matplotlib.pyplot as plt

def read_1_1_data(lines):
    data = dict()

    # Skip the first line which is the header
    for line in lines[1:]:
        # Skip empty lines
        if not line.strip():
            continue

        parts = line.split()
        num_threads = int(parts[0])
        num_throws = int(parts[1])
        # make num_theads a 10^x number (for example if num_threads is 1000 make it 10^3)
        num_throws = int(np.log10(num_throws))
        num_throws = "10^" + str(num_throws)

        serial_time = float(parts[2])
        pi_serial = float(parts[3])
        parallel_time = float(parts[4])
        pi_parallel = float(parts[5])

        # Add the data[threads][throws] = [(serial_time, parallel_time)]
        if num_threads not in data:
            data[num_threads] = [0, 0]

        data[num_threads][0] += serial_time
        data[num_threads][1] += parallel_time

    for threads in data:
        data[threads][0] /= 4
        data[threads][1] /= 4

    return data


def read_1_2_data(lines):
    data = dict()

    # Skip the first line which is the header
    for line in lines[1:]:
        # Skip empty lines
        if not line.strip():
            continue

        parts = line.split()
        num_threads = int(parts[0])
        num_iterations = int(parts[1])
        time = float(parts[2])
        sync_method = parts[-1]

        if num_threads not in data:
            data[num_threads] = [0, 0]
        
        if sync_method == "Atomics":
            data[num_threads][0] += time
        elif sync_method == "Mutexes":
            data[num_threads][1] += time

    # Divide the sum of times by 4 to get the average    
    for threads in data:
        data[threads][0] /= 4
        data[threads][1] /= 4

    return data


def read_1_3_data(lines):
    data = dict()

    # Skip the first line which is the header
    for line in lines[1:]:
        # Skip empty lines
        if not line.strip():
            continue

        # Break the line into parts and convert them to the appropriate types        
        parts = line.split()

        num_threads = int(parts[0])
        iterations = int(parts[1])
        time = float(parts[2])
        sum = float(parts[3])
        using_padding = parts[4]

        if num_threads not in data:
            data[num_threads] = [0, 0]
        
        if using_padding == "0":
            data[num_threads][0] += time
        elif using_padding == "1":
            data[num_threads][1] += time

    # Divide the sum of times by 4 to get the average
    for threads in data:
        data[threads][0] /= 4
        data[threads][1] /= 4

    return data


def read_1_4_data(lines):
    data = dict()

    # Skip the first line which is the header
    for line in lines[1:]:
        # Skip empty lines
        if not line.strip():
            continue

        # Break the line into parts and convert them to the appropriate types
        parts = line.split()

        num_threads = int(parts[0])
        search_percentage = float(parts[3])
        prior = int(parts[6])
        time = float(parts[7])

        if num_threads not in data:
            data[num_threads] = dict()
        
        if search_percentage not in data[num_threads]:
            data[num_threads][search_percentage] = [0, 0]
        
        data[num_threads][search_percentage][prior] += time

    # Divide the sum of times by 4 to get the average
    for threads in data:
        for search_percentage in data[threads]:
            data[threads][search_percentage][0] /= 4
            data[threads][search_percentage][1] /= 4

    return data


def read_data(file_path, instance):
    data = []
    with open(file_path, 'r') as file:
        lines = file.readlines()

    if instance == 1:
        return read_1_1_data(lines)
    elif instance == 2:
        return read_1_2_data(lines)
    elif instance == 3:
        return read_1_3_data(lines)
    elif instance == 4:
        return read_1_4_data(lines)


def plot_data_1_1(data, graph_file):
    num_threads = list(data.keys())
    serial_times = [data[threads][0] for threads in data]
    parallel_times = [data[threads][1] for threads in data]

    x = np.arange(len(num_threads))  # the label locations
    width = 0.35  # the width of the bars

    fig, ax = plt.subplots(figsize=(12, 6))
    bars1 = ax.bar(x - width/2, serial_times, width, label='Serial Time')
    bars2 = ax.bar(x + width/2, parallel_times, width, label='Parallel Time')

    ax.set_xlabel('Number of Threads')
    ax.set_ylabel('Elapsed Time (seconds)')
    ax.set_title('Elapsed Time for Serial and Parallel Execution')
    ax.set_xticks(x)
    ax.set_xticklabels(num_threads)
    ax.legend()

    ax.bar_label(bars1, padding=3)
    ax.bar_label(bars2, padding=3)

    fig.tight_layout()
    plt.grid(True)
    plt.savefig(graph_file)
    plt.show()



def plot_data_1_2(data, graph_file):
    num_threads = list(data.keys())
    time_atomics = [data[threads][0] for threads in data]
    time_mutexes = [data[threads][1] for threads in data]

    x = np.arange(len(num_threads))  # the label locations
    width = 0.35  # the width of the bars

    fig, ax = plt.subplots(figsize=(12, 6))
    bars1 = ax.bar(x - width/2, time_atomics, width, label='Atomics')
    bars2 = ax.bar(x + width/2, time_mutexes, width, label='Mutexes')

    ax.set_xlabel('Number of Threads')
    ax.set_ylabel('Elapsed Time (seconds)')
    ax.set_title('Elapsed Time for Atomics and Mutexes')

    ax.set_xticks(x)
    ax.set_xticklabels(num_threads)
    ax.legend()

    ax.bar_label(bars1, padding=3)
    ax.bar_label(bars2, padding=3)

    fig.tight_layout()
    plt.grid(True)
    plt.savefig(graph_file)


def plot_data_1_3(data, graph_file):
    num_threads = list(data.keys())
    time_without_padding = [data[threads][0] for threads in data]
    time_with_padding = [data[threads][1] for threads in data]

    x = np.arange(len(num_threads))  # the label locations
    width = 0.35  # the width of the bars

    fig, ax = plt.subplots(figsize=(12, 6))
    bars1 = ax.bar(x - width/2, time_without_padding, width, label='Without Padding')
    bars2 = ax.bar(x + width/2, time_with_padding, width, label='With Padding')

    ax.set_xlabel('Number of Threads')
    ax.set_ylabel('Elapsed Time (seconds)')
    ax.set_title('Elapsed Time for Padding and Without Padding')

    ax.set_xticks(x)
    ax.set_xticklabels(num_threads)
    ax.legend()

    ax.bar_label(bars1, padding=3)
    ax.bar_label(bars2, padding=3)

    fig.tight_layout()
    plt.grid(True)
    plt.savefig(graph_file)


def plot_data_1_4(data, graph_file):
    num_threads = list(data.keys())

    # Percentages are the same for all of the threads
    percentages = list(data[num_threads[0]].keys())

    for percentage in percentages:
        time_readers = [data[threads][percentage][0] for threads in data]
        time_writers = [data[threads][percentage][1] for threads in data]

        x = np.arange(len(num_threads))  # the label locations
        width = 0.35  # the width of the bars

        fig, ax = plt.subplots(figsize=(12, 6))
        bars1 = ax.bar(x - width/2, time_readers, width, label='Readers')
        bars2 = ax.bar(x + width/2, time_writers, width, label='Writers')

        ax.set_xlabel('Number of Threads')
        ax.set_ylabel('Elapsed Time (seconds)')
        ax.set_title(f'Elapsed Time for Readers and Writers {percentage * 100}% Member')

        ax.set_xticks(x)
        ax.set_xticklabels(num_threads)
        ax.legend()

        ax.bar_label(bars1, padding=3)
        ax.bar_label(bars2, padding=3)

        fig.tight_layout()
        plt.grid(True)
        plt.savefig(graph_file.replace('.png', f'_{percentage}.png'))
        


def plot_data(data, graph_file, instance):
    if instance == 1:
        plot_data_1_1(data, graph_file)
    elif instance == 2:
        plot_data_1_2(data, graph_file)
    elif instance == 3:
        plot_data_1_3(data, graph_file)
    elif instance == 4:
        plot_data_1_4(data, graph_file)



if __name__ == "__main__":
    results_folder = 'results/'
    results_files = [
        ['1_1_table.txt', '1_1_graph.png'],
        ['1_2_table.txt', '1_2_graph.png'],
        ['1_3_table.txt', '1_3_graph.png'],
        ['1_4_table.txt', '1_4_graph.png']
    ]
    for text_file, graph_file in results_files:
        text_file = results_folder + text_file
        graph_file = results_folder + graph_file

        if not os.path.exists(text_file):
            print(f"File {text_file} does not exist")
            continue

        instance = int(text_file.split('_')[1])
        data = read_data(text_file, instance)
        plot_data(data, graph_file, instance)