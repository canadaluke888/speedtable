#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define CELL_PADDING 2
#define OUTPUT_BUFFER_SIZE 1000000  // 1MB buffer for large tables

// Function to compute the maximum column widths
void compute_column_widths(char ***data, int rows, int cols, int *col_widths) {
    for (int j = 0; j < cols; j++) {
        col_widths[j] = 0;
        for (int i = 0; i < rows; i++) {
            if (data[i][j] != NULL) {
                wchar_t wstr[256];
                mbstowcs(wstr, data[i][j], 256);
                int len = wcswidth(wstr, wcslen(wstr)) + CELL_PADDING;

                if (len > col_widths[j]) {
                    col_widths[j] = len;
                }
            }
        }
    }
}

// Function to render the table
void render_table(char ***data, int rows, int cols, char *output) {
    int col_widths[cols];
    compute_column_widths(data, rows, cols, col_widths);

    // Ensure buffer is empty
    memset(output, 0, OUTPUT_BUFFER_SIZE);

    // Top border
    strcat(output, "┏");
    for (int j = 0; j < cols; j++) {
        for (int k = 0; k < col_widths[j]; k++) strcat(output, "━");
        strcat(output, (j < cols - 1) ? "┳" : "┓");
    }
    strcat(output, "\n");

    // Table content
    for (int i = 0; i < rows; i++) {
        strcat(output, "┃");
        for (int j = 0; j < cols; j++) {
            strcat(output, " ");
            strcat(output, data[i][j]);

            // Convert to wide char and compute width
            wchar_t wstr[256];
            mbstowcs(wstr, data[i][j], 256);
            int used_width = wcswidth(wstr, wcslen(wstr));

            // Pad spaces to match column width
            for (int k = used_width; k < col_widths[j] - 1; k++) strcat(output, " ");
            
            strcat(output, "┃");  // Ensure separator is placed correctly
        }
        strcat(output, "\n");

        // Row separator (if not last row)
        if (i < rows - 1) {
            strcat(output, "┣");
            for (int j = 0; j < cols; j++) {
                for (int k = 0; k < col_widths[j]; k++) strcat(output, "━");
                strcat(output, (j < cols - 1) ? "╋" : "┫");
            }
            strcat(output, "\n");
        }
    }

    // Bottom border
    strcat(output, "┗");
    for (int j = 0; j < cols; j++) {
        for (int k = 0; k < col_widths[j]; k++) strcat(output, "━");
        strcat(output, (j < cols - 1) ? "┻" : "┛");
    }
    strcat(output, "\n");
}



// Python wrapper function
static PyObject* py_render_table(PyObject* self, PyObject* args) {
    PyObject *table_data;
    int rows, cols;

    if (!PyArg_ParseTuple(args, "Oii", &table_data, &rows, &cols)) {
        return NULL;
    }

    // Allocate memory for table data
    char ***data = (char ***)malloc(rows * sizeof(char **));
    if (!data) return PyErr_NoMemory();
    
    for (int i = 0; i < rows; i++) {
        data[i] = (char **)malloc(cols * sizeof(char *));
        if (!data[i]) return PyErr_NoMemory();
        
        PyObject *row = PyList_GetItem(table_data, i);
        if (!row || !PyList_Check(row)) {
            return PyErr_Format(PyExc_TypeError, "Row %d is not a list", i);
        }
        
        for (int j = 0; j < cols; j++) {
            PyObject *cell = PyList_GetItem(row, j);
            if (!cell) {
                return PyErr_Format(PyExc_TypeError, "Expected value at row %d, col %d", i, j);
            }

            // Convert non-string values to strings
            PyObject *str_obj = PyObject_Str(cell);
            if (!str_obj) {
                return PyErr_Format(PyExc_TypeError, "Failed to convert value at row %d, col %d", i, j);
            }

            data[i][j] = strdup(PyUnicode_AsUTF8(str_obj));
            Py_DECREF(str_obj);  // Free temporary string object
        }
    }

    // Allocate a large buffer for output
    char *output = malloc(OUTPUT_BUFFER_SIZE);
    if (!output) return PyErr_NoMemory();

    render_table(data, rows, cols, output);

    // Free allocated memory
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            free(data[i][j]);
        }
        free(data[i]);
    }
    free(data);

    PyObject *result = PyUnicode_FromString(output);
    free(output);  // Free buffer after converting to Python string
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
