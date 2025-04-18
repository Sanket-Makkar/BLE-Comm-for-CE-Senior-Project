import sys
import re
def count_commas(text):
    text = text.replace('[', '').replace(']', '')
    text = text.replace('\n', '').replace('\r', '')

    text_no_commas = text.replace(',', '')

    float_strings = re.findall(r'\d+\.\d+', text_no_commas)
    floats = [float(f) for f in float_strings]
    
    return len(floats), floats


# Example usage
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python analyzecontents.py <input_string>")
        sys.exit(1)
    filename = sys.argv[1]
    try:
        with open(filename, 'r') as file:
            input_str = file.read()
            print(count_commas(input_str))
    except:
        print("failed analysis")
