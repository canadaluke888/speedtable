#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <regex.h>

#define CELL_PADDING 2
#define OUTPUT_BUFFER_SIZE 1000000  // 1MB buffer for large tables

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
    
    if (strcmp(color_name, "bold_red") == 0) return "\033[1;31m";  // Only used for headers
    if (strcmp(color_name, "bold_green") == 0) return "\033[1;32m";
    if (strcmp(color_name, "bold_yellow") == 0) return "\033[1;33m";
    if (strcmp(color_name, "bold_blue") == 0) return "\033[1;34m";
    if (strcmp(color_name, "bold_magenta") == 0) return "\033[1;35m";
    if (strcmp(color_name, "bold_cyan") == 0) return "\033[1;36m";
    if (strcmp(color_name, "bold_white") == 0) return "\033[1;37m";

    return "\033[0m";  // Default: Reset color
}


// Function to compute the maximum column widths
void compute_column_widths(char ***data, int rows, int cols, int *col_widths) {
    for (int j = 0; j < cols; j++) {
        col_widths[j] = 0;
        for (int i = 0; i < rows; i++) {
            if (data[i][j] != NULL) {
                wchar_t wstr[256];
                mbstowcs(wstr, data[i][j], 256);
                int len = wcswidth(wstr, wcslen(wstr));

                // 🔹 If this is the header row, add an extra space for bold text handling
                if (i == 0) {
                    len += 1;
                }

                // 🔹 Ensure at least 1 space of padding between columns
                len += 2; 

                if (len > col_widths[j]) {
                    col_widths[j] = len;
                }
            }
        }
        // Debug to check final column width
        printf("Column %d FINAL width: %d\n", j, col_widths[j]);
    }
}

// Function to render the table
void render_table(
    char ***data, int rows, int cols, char *output,
    const char *header_color_name, const char *border_color_name, const char *body_color_name
) {
    int col_widths[cols];
    compute_column_widths(data, rows, cols, col_widths);

    memset(output, 0, OUTPUT_BUFFER_SIZE);

    // Convert color names to ANSI codes
    const char *header_color = get_ansi_code(header_color_name, 1);  // Always bold
    const char *border_color = get_ansi_code(border_color_name, 0);
    const char *body_color = get_ansi_code(body_color_name, 0);  // Never bold

    // Debug: Print column widths
    printf("\nDEBUG - Column widths: ");
    for (int j = 0; j < cols; j++) {
        printf("[%d] ", col_widths[j]);
    }
    printf("\n");

    // Top border
    strcat(output, border_color);
    strcat(output, "┏");
    for (int j = 0; j < cols; j++) {
        for (int k = 0; k < col_widths[j]; k++) strcat(output, "━");
        strcat(output, (j < cols - 1) ? "┳" : "┓");
    }
    strcat(output, "\033[0m\n");

    // Table content
    for (int i = 0; i < rows; i++) {
        strcat(output, border_color);
        strcat(output, (i == 0) ? "┃" : "│");
        strcat(output, "\033[0m");

        for (int j = 0; j < cols; j++) {
            strcat(output, " ");

            // Header row (always bold)
            if (i == 0) {
                strcat(output, header_color);  
            } else {
                strcat(output, body_color);
            }

            strcat(output, data[i][j]);
            strcat(output, "\033[0m");  // Reset color after text

            // Compute text width and adjust spacing
            wchar_t wstr[256];
            mbstowcs(wstr, data[i][j], 256);
            int used_width = wcswidth(wstr, wcslen(wstr));

            // Ensure proper padding
            for (int k = used_width; k < col_widths[j] - 2; k++) strcat(output, " ");
            
            strcat(output, border_color);
            strcat(output, (i == 0) ? " ┃" : " │");
            strcat(output, "\033[0m");
        }
        strcat(output, "\n");

        // Header separator (HEAVY)
        if (i == 0) {
            strcat(output, border_color);
            strcat(output, "┡");
            for (int j = 0; j < cols; j++) {
                for (int k = 0; k < col_widths[j]; k++) strcat(output, "━");
                strcat(output, (j < cols - 1) ? "╇" : "┩");
            }
            strcat(output, "\033[0m\n");
        }
        // Body row separators (LIGHT)
        else if (i < rows - 1) {
            strcat(output, border_color);
            strcat(output, "├");
            for (int j = 0; j < cols; j++) {
                for (int k = 0; k < col_widths[j]; k++) strcat(output, "─");
                strcat(output, (j < cols - 1) ? "┼" : "┤");
            }
            strcat(output, "\033[0m\n");
        }
    }

    // Bottom border
    strcat(output, border_color);
    strcat(output, "└");
    for (int j = 0; j < cols; j++) {
        for (int k = 0; k < col_widths[j]; k++) strcat(output, "─");
        strcat(output, (j < cols - 1) ? "┴" : "┘");
    }
    strcat(output, "\033[0m\n");
}

// Python wrapper function
static PyObject* py_render_table(PyObject* self, PyObject* args) {
    PyObject *table_data;
    const char *header_color, *border_color, *body_color;

    if (!PyArg_ParseTuple(args, "Osss", &table_data, &header_color, &border_color, &body_color)) {
        return NULL;
    }

    // Extract columns list
    PyObject *columns = PyDict_GetItemString(table_data, "columns");
    if (!columns || !PyList_Check(columns)) {
        return PyErr_Format(PyExc_TypeError, "'columns' must be a list");
    }

    int cols = PyList_Size(columns);
    char **col_names = (char **)malloc(cols * sizeof(char *));
    for (int j = 0; j < cols; j++) {
        PyObject *col = PyList_GetItem(columns, j);
        PyObject *name = PyDict_GetItemString(col, "name");
        PyObject *type = PyDict_GetItemString(col, "type");
        
        if (!name || !PyUnicode_Check(name) || !type || !PyUnicode_Check(type)) {
            return PyErr_Format(PyExc_TypeError, "Columns must have 'name' and 'type' as strings");
        }

        // Format column header as "Name (type)"
        const char *col_name_str = PyUnicode_AsUTF8(name);
        const char *col_type_str = PyUnicode_AsUTF8(type);
        char full_col_name[100];
        snprintf(full_col_name, sizeof(full_col_name), "%s (%s)", col_name_str, col_type_str);
        
        col_names[j] = strdup(full_col_name);
    }

    // Extract rows list
    PyObject *rows = PyDict_GetItemString(table_data, "rows");
    if (!rows || !PyList_Check(rows)) {
        return PyErr_Format(PyExc_TypeError, "'rows' must be a list");
    }

    int num_rows = PyList_Size(rows);
    char ***data = (char ***)malloc((num_rows + 1) * sizeof(char **));

    // Store column headers with types
    data[0] = (char **)malloc(cols * sizeof(char *));
    for (int j = 0; j < cols; j++) {
        data[0][j] = strdup(col_names[j]);
    }

    // Store row data
    for (int i = 0; i < num_rows; i++) {
        PyObject *row = PyList_GetItem(rows, i);
        if (!row || !PyDict_Check(row)) {
            return PyErr_Format(PyExc_TypeError, "Each row must be a dictionary");
        }

        data[i + 1] = (char **)malloc(cols * sizeof(char *));
        for (int j = 0; j < cols; j++) {
            PyObject *value = PyDict_GetItemString(row, col_names[j]);
            if (!value) {
                return PyErr_Format(PyExc_KeyError, "Missing key '%s' in row %d", col_names[j], i);
            }

            PyObject *str_value = PyObject_Str(value);
            data[i + 1][j] = strdup(PyUnicode_AsUTF8(str_value));
            Py_DECREF(str_value);
        }
    }

    // Allocate output buffer
    char *output = malloc(OUTPUT_BUFFER_SIZE);
    if (!output) return PyErr_NoMemory();

    // Call render_table with updated headers
    render_table(data, num_rows + 1, cols, output, header_color, border_color, body_color);

    // Free allocated memory
    for (int i = 0; i <= num_rows; i++) {
        for (int j = 0; j < cols; j++) {
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);
    free(col_names);

    PyObject *result = PyUnicode_FromString(output);
    free(output);
    return result;
}




// Define method mappings
static PyMethodDef TableMethods[] = {
    {"render_table", py_render_table, METH_VARARGS, "Render table in C"},
    {NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef tablemodule = {
    PyModuleDef_HEAD_INIT,
    "table_render",
    NULL,
    -1,
    TableMethods
};

// Initialize the Python module
PyMODINIT_FUNC PyInit_table_render(void) {
    return PyModule_Create(&tablemodule);
}
