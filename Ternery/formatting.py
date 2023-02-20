import sys

def main():
    ###need to parse the weights file properly for c
    #need to take out '[' ']' '.' chars
    argumentList = sys.argv[1:]
    #fd = open(str(argumentList[0]), "r")
    #formatted_file = open(str(argumentList[0]) - ".txt" + ".formatted.txt ", "w")
    fd = open("D:\\HW\\project\\Ternary-Weights-Network-master\\Params\\fc1.weight.txt", "r")
    formatted_file = open("D:\\HW\\project\\Ternary-Weights-Network-master\\Params\\fc1.weight.formatted.txt", "w")
    list_of_lines = fd.readlines()
    new_list = list()
    for line in list_of_lines:
        new_list.append(line.replace(".", "").replace("[", "").replace("]", ""))
    #write to formatted file
    formatted_file.writelines(new_list)
    fd.close()
    formatted_file.close()

if __name__ == "__main__":
    main()