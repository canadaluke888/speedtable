#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define INITIAL_CAPACITY 128

// Dynamic buffer structure for dynamic string building
typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} dyn_buf;

// Initialize dynamic buffer with an initial capacity.
void dyn_buf_init(dyn_buf *db) {
    db->capacity = INITIAL_CAPACITY;
    db->length = 0;
    db->data = (char *)malloc(db->capacity);
    if (db->data) {
        db->data[0] = '\0';
    }
}

// Append a string to the dynamic buffer, reallocating if needed.
void dyn_buf_append(dyn_buf *db, const char *str) {
    size_t add_len = strlen(str);
    while (db->length + add_len + 1 > db->capacity) {
        db->capacity *= 2;
        db->data = (char *)realloc(db->data, db->capacity);
    }
    strcpy(db->data + db->length, str);
    db->length += add_len;
}

// Free the dynamic buffer memory.
void dyn_buf_free(dyn_buf *db) {
    if (db->data) {
        free(db->data);
    }
}

// Color mapping to ANSI code
const char* get_ansi_code(const char* color_name, int is_header) {
    if (strcmp(color_name, "black") == 0) return is_header ? "\033[1;30m" : "\033[0;30m";
    if (strcmp(color_name, "red") == 0) return is_header ? "\033[1;31m" : "\033[0;31m";
    if (strcmp(color_name, "green") == 0) return is_header ? "\033[1;32m" : "\033[0;32m";
    if (strcmp(color_name, "yellow") == 0) return is_header ? "\033[1;33m" : "\033[0;33m";
    if (strcmp(color_name, "blue") == 0) return is_header ? "\033[1;34m" : "\033[0;34m";
    if (strcmp(color_name, "magenta") == 0) return is_header ? "\033[1;35m" : "\033[0;35m";
    if (strcmp(color_name, "cyan") == 0) return is_header ? "\033[1;36m" : "\033[0;36m";
    if (strcmp(color_name, "white") == 0) return is_header ? "\033[1;37m" : "\033[0;37m";
    
    if (strcmp(color_name, "bold_red") == 0) return "\033[1;31m";
    if (strcmp(color_name, "bold_green") == 0) return "\033[1;32m";
    if (strcmp(color_name, "bold_yellow") == 0) return "\033[1;33m";
    if (strcmp(color_name, "bold_blue") == 0) return "\033[1;34m";
    if (strcmp(color_name, "bold_magenta") == 0) return "\033[1;35m";
    if (strcmp(color_name, "bold_cyan") == 0) return "\033[1;36m";
    if (strcmp(color_name, "bold_white") == 0) return "\033[1;37m";

    return "\033[0m";  // Default: Reset color
}

// Function to compute the maximum column widths.
void compute_column_widths(char ***data, int rows, int cols, int *col_widths) {
    for (int j = 0; j < cols; j++) {
        int max_width = 0;
        for (int i = 0; i < rows; i++) {
            wchar_t wstr[256];
            mbstowcs(wstr, data[i][j], 256);
            int used_width = wcswidth(wstr, wcslen(wstr));
            if (used_width > max_width) {
                max_width = used_width;
            }
        }
        col_widths[j] = max_width + 2;  // Add padding
    }
}

// Function to render the table into a dynamic buffer.
void render_table(
    char ***data, int rows, int cols, dyn_buf *out,
    const char *header_color_name, const char *border_color_name,
    const char *body_color_name, const char *type_color_name,
    const char *title, const char *title_color
) {
    int col_widths[cols];
    compute_column_widths(data, rows, cols, col_widths);
    out->length = 0;
    if (out->data)
        out->data[0] = '\0';

    const char *header_color = get_ansi_code(header_color_name, 1);
    const char *border_color = get_ansi_code(border_color_name, 0);
    const char *body_color = get_ansi_code(body_color_name, 0);
    const char *type_color = get_ansi_code(type_color_name, 0);
    const char *title_color_code = get_ansi_code(title_color, 0);

    int total_width = 1; // For the left border
    for (int j = 0; j < cols; j++) {
        total_width += col_widths[j];
        total_width += 1; // For the separators
    }

    // Title line (centered, italic, colored)
    if (strlen(title) > 0) {
        int title_len = strlen(title);
        int padding = (total_width - title_len) / 2;
        for (int i = 0; i < padding; i++) dyn_buf_append(out, " ");
        dyn_buf_append(out, title_color_code);
        dyn_buf_append(out, "\033[3m");  // Italic
        dyn_buf_append(out, title);
        dyn_buf_append(out, "\033[0m\n");
    }

    // Top border
    dyn_buf_append(out, border_color);
    dyn_buf_append(out, "┏");
    for (int j = 0; j < cols; j++) {
        for (int k = 0; k < col_widths[j]; k++) dyn_buf_append(out, "━");
        dyn_buf_append(out, (j < cols - 1) ? "┳" : "┓");
    }
    dyn_buf_append(out, "\033[0m\n");

    // Table content
    for (int i = 0; i < rows; i++) {
        dyn_buf_append(out, border_color);
        dyn_buf_append(out, (i == 0) ? "┃" : "│");
        dyn_buf_append(out, "\033[0m");
        for (int j = 0; j < cols; j++) {
            dyn_buf_append(out, " ");
            if (i == 0) {
                // Header row
                char *header = data[i][j];
                char *paren = strchr(header, '(');
                if (paren) {
                    size_t name_len = paren - header;
                    char col_name[100];
                    strncpy(col_name, header, name_len);
                    col_name[name_len] = '\0';
                    dyn_buf_append(out, header_color);
                    dyn_buf_append(out, col_name);
                    dyn_buf_append(out, "\033[0m");
                    dyn_buf_append(out, type_color);
                    dyn_buf_append(out, paren);
                    dyn_buf_append(out, "\033[0m");
                } else {
                    dyn_buf_append(out, header_color);
                    dyn_buf_append(out, header);
                    dyn_buf_append(out, "\033[0m");
                }
            } else {
                // Body rows
                dyn_buf_append(out, body_color);
                dyn_buf_append(out, data[i][j]);
                dyn_buf_append(out, "\033[0m");
            }
            wchar_t wstr[256];
            mbstowcs(wstr, data[i][j], 256);
            int used_width = wcswidth(wstr, wcslen(wstr));
            for (int k = used_width; k < col_widths[j] - 2; k++) dyn_buf_append(out, " ");
            dyn_buf_append(out, border_color);
            dyn_buf_append(out, (i == 0) ? " ┃" : " │");
            dyn_buf_append(out, "\033[0m");
        }
        dyn_buf_append(out, "\n");
        if (i == 0) {
            dyn_buf_append(out, border_color);
            dyn_buf_append(out, "┡");
            for (int j = 0; j < cols; j++) {
                for (int k = 0; k < col_widths[j]; k++) dyn_buf_append(out, "━");
                dyn_buf_append(out, (j < cols - 1) ? "╇" : "┩");
            }
            dyn_buf_append(out, "\033[0m\n");
        } else if (i < rows - 1) {
            dyn_buf_append(out, border_color);
            dyn_buf_append(out, "├");
            for (int j = 0; j < cols; j++) {
                for (int k = 0; k < col_widths[j]; k++) dyn_buf_append(out, "─");
                dyn_buf_append(out, (j < cols - 1) ? "┼" : "┤");
            }
            dyn_buf_append(out, "\033[0m\n");
        }
    }

    // Bottom border
    dyn_buf_append(out, border_color);
    dyn_buf_append(out, "└");
    for (int j = 0; j < cols; j++) {
        for (int k = 0; k < col_widths[j]; k++) dyn_buf_append(out, "─");
        dyn_buf_append(out, (j < cols - 1) ? "┴" : "┘");
    }
    dyn_buf_append(out, "\033[0m\n");
}

// Python wrapper function.
static PyObject* py_render_table(PyObject* self, PyObject* args) {
    PyObject *table_data;
    const char *header_color, *border_color, *body_color, *type_color;
    const char *title_text, *title_color;
    if (!PyArg_ParseTuple(args, "Ossssss", &table_data, &header_color, &border_color, &body_color, &type_color, &title_text, &title_color)) {
        return NULL;
    }
    PyObject *columns = PyDict_GetItemString(table_data, "columns");
    if (!columns || !PyList_Check(columns)) {
        return PyErr_Format(PyExc_TypeError, "'columns' must be a list");
    }
    int cols = PyList_Size(columns);
    char **col_keys = (char **)malloc(cols * sizeof(char *));
    char **col_headers = (char **)malloc(cols * sizeof(char *));
    char **col_types = (char **)malloc(cols * sizeof(char *));
    for (int j = 0; j < cols; j++) {
        PyObject *col = PyList_GetItem(columns, j);
        PyObject *name = PyDict_GetItemString(col, "name");
        PyObject *type = PyDict_GetItemString(col, "type");
        if (!name || !PyUnicode_Check(name) || !type || !PyUnicode_Check(type)) {
            return PyErr_Format(PyExc_TypeError, "Each column must have a string 'name' and 'type'");
        }
        const char *col_name_str = PyUnicode_AsUTF8(name);
        const char *col_type_str = PyUnicode_AsUTF8(type);
        col_keys[j] = strdup(col_name_str);
        col_types[j] = strdup(col_type_str);
        char full_header[100];
        snprintf(full_header, sizeof(full_header), "%s (%s)", col_name_str, col_type_str);
        col_headers[j] = strdup(full_header);
    }
    PyObject *rows = PyDict_GetItemString(table_data, "rows");
    if (!rows || !PyList_Check(rows)) {
        return PyErr_Format(PyExc_TypeError, "'rows' must be a list");
    }
    int num_rows = PyList_Size(rows);
    char ***data = (char ***)malloc((num_rows + 1) * sizeof(char **));
    data[0] = (char **)malloc(cols * sizeof(char *));
    for (int j = 0; j < cols; j++) {
        data[0][j] = strdup(col_headers[j]);
    }
    for (int i = 0; i < num_rows; i++) {
        PyObject *row = PyList_GetItem(rows, i);
        if (!row || !PyDict_Check(row)) {
            return PyErr_Format(PyExc_TypeError, "Each row must be a dictionary");
        }
        data[i + 1] = (char **)malloc(cols * sizeof(char *));
        for (int j = 0; j < cols; j++) {
            PyObject *value = PyDict_GetItemString(row, col_keys[j]);
            if (!value) {
                return PyErr_Format(PyExc_KeyError, "Missing key '%s' in row %d", col_keys[j], i);
            }
            PyObject *str_value = PyObject_Str(value);
            data[i + 1][j] = strdup(PyUnicode_AsUTF8(str_value));
            Py_DECREF(str_value);
        }
    }
    dyn_buf out;
    dyn_buf_init(&out);
    render_table(data, num_rows + 1, cols, &out, header_color, border_color, body_color, type_color, title_text, title_color);
    for (int i = 0; i <= num_rows; i++) {
        for (int j = 0; j < cols; j++) {
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);
    for (int j = 0; j < cols; j++) {
        free(col_keys[j]);
        free(col_headers[j]);
        free(col_types[j]);
    }
    free(col_keys);
    free(col_headers);
    free(col_types);
    PyObject *result = PyUnicode_FromString(out.data);
    dyn_buf_free(&out);
    return result;
}

// Define method mappings.
static PyMethodDef TableMethods[] = {
    {"render_table", py_render_table, METH_VARARGS, "Render table in C"},
    {NULL, NULL, 0, NULL}
};

// Module definition.
static struct PyModuleDef tablemodule = {
    PyModuleDef_HEAD_INIT,
    "table_render",
    NULL,
    -1,
    TableMethods
};

// Initialize the Python module.
PyMODINIT_FUNC PyInit_speedtable(void) {
    return PyModule_Create(&tablemodule);
}
