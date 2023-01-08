import re
import textwrap
import sys

ROW_MAX_LENGTH = 23

def computeLength(str):
    return len(str)

def handleReplic(str):
    str = str.replace("[SBREAK]","")
    
    segmentsArr = str.split("[PAUSE]")

    for i,segmentStr in enumerate(segmentsArr):

        paragsArr = segmentStr.split("[HBREAK]")

        for j,paragStr in enumerate(paragsArr):

            wordsArr = re.split(r'\s(?:(?=(?:(?![\]\)}]).)*[\[{])|(?!.*[\]}]))', paragStr)
            #wordsArr = re.split(" ", paragStr)

            newStr = wordsArr[0]
            rowLength = computeLength(newStr)

            for wordStr in wordsArr[1:]:
                wordLength = computeLength(wordStr)

                if (rowLength + 1 + wordLength > ROW_MAX_LENGTH):
                    newStr += "[BREAK]"
                    rowLength = 0

                newStr += " " + wordStr
                rowLength += 1 + wordLength

            paragsArr[j] = newStr

        segmentsArr[i] = "[BREAK]".join(paragsArr)

    return "[PAUSE]".join(segmentsArr)


#replicStr = "@Brother, here's some juice.[BREAK][PAUSE]@You are thirsty, aren't you?[BREAK][END]"
#replicStr = "@J'ai déposé [23 15 74 03 00]$[HBREAK] sur ton compte bancaire."
#replicStr = "@Reviens à la maison dès[BREAK] que tu auras une envie[BREAK] d(03 5F)[21 89 76].[BREAK][END]"
#print(handleReplic(replicStr))

#sys.exit()

inputFile = open(sys.argv[1],"r",encoding="ISO-8859-1")
outputFile = open(sys.argv[2],"w",encoding="ISO-8859-1")
lines = inputFile.readlines()
for line in lines:
    match = re.match("^([0-9A-F]{3}-E: )(.*)$", line)
    if match:
        outputFile.write(match.group(1) + handleReplic(match.group(2)) + '\n')
    else:
        outputFile.write(line)

