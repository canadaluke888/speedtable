from rich.console import Console
from rich.table import Table


console = Console()

rows = [["1", "John", "23"], ["2", "Henry", "45"]]

table = Table(title="Test")

table.add_column("ID")
table.add_column("Name")
table.add_column("Age")

for row in rows:
    table.add_row(row)


console.print(table)