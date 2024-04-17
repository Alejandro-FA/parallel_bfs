import re
import os
import pandas as pd


# Function to read data from a single input file and return a DataFrame
def create_dataframe(input_path: str, problem_type: str, goal_check_delay: int) -> pd.DataFrame:
    algorithm_map = {
        "AnyOfBFS": "C++ any_of",
        "AsyncBFS": "Recursive async",
        "TasksBFS": "Using C++ tasks",
        "ForeachBFS": "Recursive C++ for_each",
        "SyncBFS": "Sequential",
        "AsyncStartBFS": "Using C++ async",
        "MultithreadBFS": "Synchronization with thread director",
        "ForeachStartBFS": "Using C++ for_each",
    }

    # Initialize an empty dictionary to store results
    results = {'problem_type': [], 'goal_check_delay': [], 'problem_number': [], 'execution_time': [], 'algorithm_name': [], 'solution_found': []}

    # Open and read the raw data file
    with open(input_path, 'r') as file:
        # Initialize variables to store current problem number and algorithm name
        current_problem = None
        
        # Iterate through each line in the file
        for line in file:
            # Check if the line corresponds to a problem definition
            if line.startswith("problem_"):
                # Extract the problem number from the line
                current_problem = int(re.findall(r'\d+', line)[0])

            # Check if we reached the end of the data section
            elif line.startswith("[INFO]"):
                break

            # Check if the line contains execution time for an algorithm
            elif ":" in line:
                # Split the line into algorithm name and execution time
                parts = line.strip().split(":")
                algorithm_name = parts[0]
                algorithm_name = algorithm_map[algorithm_name]
                execution_time = float(parts[1].split()[0])
                solution_found = False if "No solution found!" in line else True
                
                # Store the execution time in the corresponding position of the list
                results['problem_type'].append(problem_type)
                results['goal_check_delay'].append(goal_check_delay)
                results['algorithm_name'].append(algorithm_name)
                results['problem_number'].append(current_problem)
                results['execution_time'].append(execution_time)
                results['solution_found'].append(solution_found)

    print(f"Processed {len(results['problem_number'])} results from {input_path}")
    print(f"Problem type: {problem_type}, Goal check delay: {goal_check_delay}")
    return pd.DataFrame(results)


# Function to update the existing DataFrame with new data
def update_dataframe(existing_df: pd.DataFrame, new_data_df: pd.DataFrame):
    if existing_df is None:
        return new_data_df
    else:
        result = pd.concat([existing_df, new_data_df], ignore_index=True, sort=False)
        return result


###############################################################################
# Parse data
###############################################################################
# Define the path to the raw data file
PROBLEM_TYPE = "unbalanced"
GOAL_CHECK_DELAY = 100
INPUT_PATH = "problems/unbalanced/results_2024-03-04-16:47:02.log"
DATAFRAME_PATH = "data.csv"

# Load the existing DataFrame from the file (if it exists)
df = None
if os.path.exists(DATAFRAME_PATH):
    df = pd.read_csv(DATAFRAME_PATH)

# Create a DataFrame from the raw data file
new_df = create_dataframe(INPUT_PATH, PROBLEM_TYPE, GOAL_CHECK_DELAY)

# Update the existing DataFrame with the new data
updated_df = update_dataframe(df, new_df)

# Save the updated DataFrame back to the file
updated_df.to_csv(DATAFRAME_PATH, index=False)
