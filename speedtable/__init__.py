from .speedtable import render_table as _render_table

def render_table(
    table_data: dict,
    header_color: str = "bold_white",
    border_color: str = "bold_cyan",
    body_color: str = "white",
    type_color: str = "bright_black",
    title_text: str = "",
    title_color: str = "white"
) -> str:
    """
    Render a styled terminal table using a high-performance C extension.

    Args:
        table_data: Dictionary with 'columns' and 'rows'.
        header_color: Color name for column headers (always bold).
        border_color: Color name for table borders.
        body_color: Color name for cell text.
        type_color: Color name for type labels in headers (e.g., "(int)").
        title_text: Optional string to center above the table.
        title_color: Color name for the title (always italicized).

    Returns:
        A string containing the fully styled table, ready to print.
    """
    return _render_table(
        table_data,
        header_color,
        border_color,
        body_color,
        type_color,
        title_text,
        title_color
    )