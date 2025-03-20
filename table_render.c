#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <regex.h>

#define CELL_PADDING 2
#define OUTPUT_BUFFER_SIZE 1000000  // 1MB buffer for large tables

#define BOLD_START "\033[1m"
#define COLOR_BLUE "\033[34m"
#define RESET_FORMAT "\033[0m"

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
void render_table(char ***data, int rows, int cols, char *output) {
    int col_widths[cols];
    compute_column_widths(data, rows, cols, col_widths);

    memset(output, 0, OUTPUT_BUFFER_SIZE);

    // Debug: Print column widths
    printf("\nDEBUG - Column widths: ");
    for (int j = 0; j < cols; j++) {
        printf("[%d] ", col_widths[j]);
    }
    printf("\n");

    // Top border (HEAVY for header)
    strcat(output, "┏");
    for (int j = 0; j < cols; j++) {
        for (int k = 0; k < col_widths[j]; k++) strcat(output, "━");  // Heavy
        strcat(output, (j < cols - 1) ? "┳" : "┓");
    }
    strcat(output, "\n");

    // Table content
    for (int i = 0; i < rows; i++) {
        // Use heavy separator for header, light for body
        strcat(output, (i == 0) ? "┃" : "│");

        for (int j = 0; j < cols; j++) {
            strcat(output, " ");

            // Apply bold formatting only for header row
            if (i == 0) {
                strcat(output, "\033[1m"); // Start Bold
            }

            strcat(output, data[i][j]);

            // Reset formatting after header row
            if (i == 0) {
                strcat(output, "\033[0m"); // End Bold
            }

            // Compute text width and adjust spacing
            wchar_t wstr[256];
            mbstowcs(wstr, data[i][j], 256);
            int used_width = wcswidth(wstr, wcslen(wstr));

            // Ensure proper padding so vertical bars align
            for (int k = used_width; k < col_widths[j] - 2; k++) strcat(output, " ");
            
            strcat(output, (i == 0) ? " ┃" : " │");  // Heavy for header, light for body
        }
        strcat(output, "\n");

        // Separator line
        if (i == 0) {  // After the header
            strcat(output, "┣");
            for (int j = 0; j < cols; j++) {
                for (int k = 0; k < col_widths[j]; k++) strcat(output, "━");  // Heavy
                strcat(output, (j < cols - 1) ? "╋" : "┫");
            }
            strcat(output, "\n");
        } else if (i < rows - 1) {  // Between regular rows
            strcat(output, "├");
            for (int j = 0; j < cols; j++) {
                for (int k = 0; k < col_widths[j]; k++) strcat(output, "─");  // Light
                strcat(output, (j < cols - 1) ? "┼" : "┤");
            }
            strcat(output, "\n");
        }
    }

    // Bottom border (LIGHT)
    strcat(output, "└");
    for (int j = 0; j < cols; j++) {
        for (int k = 0; k < col_widths[j]; k++) strcat(output, "─");  // Light
        strcat(output, (j < cols - 1) ? "┴" : "┘");
    }
    strcat(output, "\n");
}



// Python wrapper function
static PyObject* py_render_table(PyObject* self, PyObject* args) {
    PyObject *table_data;  // Raw table dictionary

    if (!PyArg_ParseTuple(args, "O", &table_data)) {
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
        if (!name || !PyUnicode_Check(name)) {
            return PyErr_Format(PyExc_TypeError, "Column names must be strings");
        }
        col_names[j] = strdup(PyUnicode_AsUTF8(name));
    }

    // Extract rows list
    PyObject *rows = PyDict_GetItemString(table_data, "rows");
    if (!rows || !PyList_Check(rows)) {
        return PyErr_Format(PyExc_TypeError, "'rows' must be a list");
    }

    int num_rows = PyList_Size(rows);
    char ***data = (char ***)malloc((num_rows + 1) * sizeof(char **));  // +1 for column headers

    // Store column headers
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

            // Convert value to string
            PyObject *str_value = PyObject_Str(value);
            data[i + 1][j] = strdup(PyUnicode_AsUTF8(str_value));
            Py_DECREF(str_value);
        }
    }

    // Allocate output buffer
    char *output = malloc(OUTPUT_BUFFER_SIZE);
    if (!output) return PyErr_NoMemory();

    render_table(data, num_rows + 1, cols, output);  // Render table

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
