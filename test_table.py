import table_render

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

c_table = table_render.render_table(table_data)
print(c_table)
