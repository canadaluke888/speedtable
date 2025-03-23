# ⚡ speedtable

**speedtable** is an ultra-fast, C-based terminal table renderer for Python.  
Designed for large datasets and low-latency rendering — with beautiful Rich-style Unicode output.

> 💡 Perfect for CLI tools, dataset previews, or any place you need fast + styled tables.

---

## 🚀 Features

- Blazing-fast C implementation 🔥  
- Unicode box-style table formatting (like Rich’s `HEAVY_HEAD`)  
- Bold, colored headers with optional column type labels  
- Customizable:
  - Header color  
  - Border color  
  - Body text color  
  - Type label color  
  - Title text and color (italicized, centered above the table)

---

## 📦 Installation

```bash
pip install speedtable
```

---

## 🧪 Example Usage

```python
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

print(speedtable.render_table(
    table_data,
    header_color="green",
    border_color="magenta",
    body_color="white",
    type_color="red",
    title_text="Test Table",
    title_color="cyan"
))
```

---
## 📷 Output
![SpeedTable Demo](https://raw.githubusercontent.com/canadaluke888/speedtable/master/assets/speedtable-demo.png)

---

## 🎨 Supported Color Names

| Name             | Description               |
|------------------|---------------------------|
| `black`          | Standard black            |
| `red`            | Standard red              |
| `green`          | Standard green            |
| `yellow`         | Standard yellow           |
| `blue`           | Standard blue             |
| `magenta`        | Standard magenta          |
| `cyan`           | Standard cyan             |
| `white`          | Standard white            |
| `bright_black`   | Gray (dim)                |
| `bold_red`       | Bright bold red           |
| `bold_white`     | Bold white (great for headers) |

> ✨ Headers are always bold, and titles are always italicized.

---

## 💡 Why speedtable?

The Python `rich` library is beautiful, but may be too slow for rendering large tables in CLI environments.  
`speedtable` gives you the same polished aesthetic — at native speed.

---

## 📄 License

MIT © [Luke Canada](https://github.com/canadaluke888)
