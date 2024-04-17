import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd

###############################################################################
# Plots
###############################################################################

# Create a list of algorithm names sorted alphabetically
sorted_algorithms = sorted(results.keys())

# Create a boxplot for the execution time of each algorithm
plt.figure(figsize=(12, 6))
sns.boxplot(data=[results[algorithm] for algorithm in sorted_algorithms])
plt.xticks(range(len(sorted_algorithms)), sorted_algorithms, rotation=45, ha='right')
plt.ylabel("Execution Time (ms)")
plt.title("Boxplot of execution time for each algorithm")
plt.tight_layout()
plt.show()


# Create a list of algorithm names sorted alphabetically
non_recursive_algorithms = [algorithm for algorithm in sorted_algorithms if "Recursive" not in algorithm]

# Create a boxplot for the execution time of each non-recursive algorithm
plt.figure(figsize=(12, 6))
sns.boxplot(data=[results[algorithm] for algorithm in non_recursive_algorithms])
plt.xticks(range(len(non_recursive_algorithms)), non_recursive_algorithms, rotation=45, ha='right')
plt.ylabel("Execution Time (ms)")
plt.title("Boxplot of execution time for each non-recursive algorithm")
plt.tight_layout()
plt.show()

# Create a boxplot for the execution time of each non-recursive algorithm without outliers
plt.figure(figsize=(12, 6))
sns.boxplot(data=[results[algorithm] for algorithm in non_recursive_algorithms], showfliers=False)
plt.xticks(range(len(non_recursive_algorithms)), non_recursive_algorithms, rotation=45, ha='right')
plt.ylabel("Execution Time (ms)")
plt.title("Boxplot of execution time for each non-recursive algorithm")
plt.tight_layout()
plt.show()