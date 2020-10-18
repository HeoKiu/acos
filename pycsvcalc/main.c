#define PY_SSIZE_T_CLEAN

#include <Python.h>

const char *programName = "Table";

const char *solutionPythonCode = "import sys\n"
                                 "table_data = sys.stdin.read()\n"
                                 "ПРЕОБРАЗУЕМ ТАБЛИЗУ КАК СКАЗАНО В ЗАДАЧЕ"
                                 "print(table_data)\n"
                                 ;

int main(int argc, char *argv[]) {
    Py_Initialize();

    PyRun_SimpleString(solutionPythonCode);

    Py_Finalize();
    return 0;
}