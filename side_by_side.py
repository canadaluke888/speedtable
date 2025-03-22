import table_render
from rich.console import Console
from rich.table import Table


table_data = {
    "columns": [
        {"name": "ID", "type": "int"},
        {"name": "Name", "type": "str"},
        {"name": "Age", "type": "int"}
    ],
    "rows": [
        {"ID": 1, "Name": "Luke", "Age": 21},
        {"ID": 2, "Name": "Joe", "Age": 45},
        {"ID": 3, "Name": "Alice", "Age": 56}
    ]
}

c_table = table_render.render_table(table_data, "red", "green", "yellow")
print(c_table)

console = Console()

rich_table = Table(show_lines=True, style="Yellow")

rich_table.add_column("ID")
rich_table.add_column("Name")
rich_table.add_column("Age")

rich_table.add_row("1", "Luke", "21", style="Blue")
rich_table.add_row("2", "Joe", "45")
rich_table.add_row("3", "Alice", "56")

console.print(rich_table)