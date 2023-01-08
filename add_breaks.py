import re
import textwrap
import sys

LINE_TYPES = {
    "NORMAL_TEXT": {
        "max_length": 22,
        "leading_spaces": " ",
        "max_lines": 3,
        "extra_last_lines": 1
    },
    "SHORTER_TEXT": {
        "max_length": 19,
        "leading_spaces": " ",
        "max_lines": 3,
        "extra_last_lines": 1
    },    
    "SHORTER_QUESTION": {
        "max_length": 19,
        "leading_spaces": " ",
        "max_lines": 3,
        "extra_last_lines": 0
    },
    "ITEM_DESCRIPTION": {
        "max_length": 22,
        "leading_spaces": " ",
        "max_lines": 4,
        "extra_last_lines": 0
    },
    "NORMAL_ITEM": {
        "max_length": 12,
        "leading_spaces": "",
        "max_lines": 1,
        "extra_last_lines": 0
    },
    "NORMAL_ITEM": {
        "max_length": 12,
        "leading_spaces": "",
        "max_lines": 1,
        "extra_last_lines": 0
    },
    "SHORTER_ITEM": {
        "max_length": 11,
        "leading_spaces": "",
        "max_lines": 1,
        "extra_last_lines": 0
    },
    "ADDITIONAL_TEXT": {
        "max_length": 23,
        "leading_spaces": " ",
        "max_lines": 3,
        "extra_last_lines": 1
    },
    "ENEMY_DESCRIPTION": {
        "max_length": 26,
        "leading_spaces": "",
        "max_lines": 3,
        "extra_last_lines": 0
    },
    "BATTLE": {
        "max_length": 28,
        "leading_spaces": "",
        "max_lines": 3,
        "extra_last_lines": 0
    },
    "ACTION": {
        "max_length": 27,
        "leading_spaces": "",
        "max_lines": 3,
        "extra_last_lines": 0
    },
    "CREDITS": {
        "max_length": 28,
        "leading_spaces": "  ",
        "max_lines": 16,
        "extra_last_lines": 0
    },
    "EPILOGUE": {
        "max_length": 28,
        "leading_spaces": "",
        "max_lines": 4,
        "extra_last_lines": 0
    }
}

def computeLength(str):
    return len(str)

def handleReplic(str, lineType):
    str = str.replace("[SBREAK]" + lineType["leading_spaces"], " ")
    str = str.replace("[SBREAK]", "")
    
    # replic: one line in script.txt
    # segment: between two PAUSEs
    # parag: between two hard BREAKs
    # row: between two BREAKs

    segmentsArr = str.split("[PAUSE]")
    
    for i, segmentStr in enumerate(segmentsArr):
        
        isLastSegment = (i == len(segmentsArr) - 1)

        paragsArr = segmentStr.split("[HBREAK]")

        for j, paragStr in enumerate(paragsArr):

            wordsArr = re.split(r'\s(?:(?=(?:(?![\]\)}]).)*[\[{])|(?!.*[\]}]))', paragStr)
            #wordsArr = re.split(" ", paragStr)

            newStr = wordsArr[0]
            rowLength = computeLength(newStr)

            #rowCount = 0

            for wordStr in wordsArr[1:]:
                wordLength = computeLength(wordStr)

                if (rowLength + 1 + wordLength > lineType["max_length"]):
                    #rowCount += 1;
                    newStr += "[BREAK]"
                    #if (rowCount >= lineType["max_lines"]):
                    #    newStr += "[PAUSE]"
                    #    rowCount = 0
                    newStr += lineType["leading_spaces"]
                    rowLength = len(lineType["leading_spaces"])
                    newStr += wordStr
                    rowLength += wordLength
                else:
                    newStr += " " + wordStr
                    rowLength += 1 + wordLength

            paragsArr[j] = newStr

        segmentsArr[i] = "[BREAK]".join(paragsArr)
        
        # Now to add PAUSEs
        

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
    replicMatch = re.match("^([0-9A-F]{3}-E: )(.*)$", line)
    if replicMatch:
        outputFile.write(replicMatch.group(1) + handleReplic(replicMatch.group(2), curLineType) + '\n')
    else:
        typeMatch = re.match("^//\s*#TYPE=(.*)$", line)
        if (typeMatch):
            curLineType = LINE_TYPES[typeMatch.group(1)]
        outputFile.write(line)

