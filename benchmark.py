import time
import table_render
from rich.console import Console
from rich.table import Table

console = Console()

# Generate a large dataset (e.g., 1000 rows)
rows = 1000
cols = 5
data = [[f"Row {i}", f"Col {j}", i * j] for i in range(rows) for j in range(cols)]
data = [data[i:i + cols] for i in range(0, len(data), cols)]  # Reshape into 2D

# Convert non-string data to strings (Python-side safeguard)
data = [[str(cell) for cell in row] for row in data]

# Benchmark C extension
start = time.time()
c_table = table_render.render_table(data, len(data), len(data[0]))
c_time = time.time() - start

# Benchmark Rich
table = Table(title="Rich Table")
for col in range(cols):
    table.add_column(f"Col {col}")

for row in data:
    table.add_row(*row)

start = time.time()
console.print(table)
rich_time = time.time() - start

# Print benchmark results
print(f"\nC extension execution time: {c_time:.5f} seconds")
print(f"Rich execution time: {rich_time:.5f} seconds")

# Print the C-generated table (sanity check)
print("\n=== C Table Output ===")
print(c_table)
