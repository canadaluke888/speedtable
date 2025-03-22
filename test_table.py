import table_render

HEADER_COLOR = "bold_yellow"
BORDER_COLOR = "bold_blue"
BODY_COLOR = "white"

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
table_output = table_render.render_table(table_data, HEADER_COLOR, BORDER_COLOR, BODY_COLOR)

print(table_output)
