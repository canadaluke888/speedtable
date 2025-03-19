import table_render

data = [
    ["ID", "Name", "Score"],
    [1, "alice", 95],
    [2, "Bob", 85]
]

print(table_render.render_table(data, len(data), len(data[0])))
