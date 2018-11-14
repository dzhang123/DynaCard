
# Compile the program
g++ compute_shape_properties.cpp;

# Run the program and pipe its output to a python script that parses the JSON
./a.out gas_interference.csv | python -c "
import json, sys
txt = sys.stdin.read()
b=json.loads(txt)
print b
"
