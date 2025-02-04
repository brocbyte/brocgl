import sys
import os
def processGlExtFile(filename, loader):
    with open(filename) as inFile:
        lines = inFile.readlines()
        api = [line for line in lines if ' APIENTRY ' in line and 'GLAPI' in line]
        head, tail = os.path.split(filename)

        with open(os.path.join(head, "def_" + tail), "w") as outFile:
            outFile.write(f"#include \"{tail}\"\n\n")
            variable_defs = []
            func_loads = []
            for line in api:
                words = line.split()
                function_name = words[words.index("APIENTRY")+1]
                type_name = "PFN" + function_name.upper() + "PROC"
                variable_defs.append(f"inline {type_name} {function_name} = 0;\n")
                func_loads.append(f"    {function_name} = ({type_name}){loader}(\"{function_name}\");\n")

            for line in variable_defs:
                outFile.write(line)
            outFile.write("inline void loadGlFunctions() {\n")
            for line in func_loads:
                outFile.write(line)
            outFile.write("}\n\n")



if __name__ == '__main__':
    processGlExtFile(sys.argv[1], sys.argv[2])