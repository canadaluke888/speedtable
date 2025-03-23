import speedtable

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

# Call the C extension
table_output = speedtable.render_table(
    table_data,
    "blue",     # header color
    "magenta",  # border color
    "white",    # body color
    "cyan",     # type label color
    "Test",     # title text
    "red"       # title color
)

print(table_output)
