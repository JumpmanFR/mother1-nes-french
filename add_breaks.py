import re
import textwrap
import sys

ROW_MAX_LENGTH = 22

replicStr = "@Brother, here's some juice.[BREAK][PAUSE]@You are thirsty, aren't you?[BREAK][END]"
#replicStr = "@J'ai déposé [23 15 74 03 00]$[HBREAK] sur ton compte bancaire."

def computeLength(str):
    return len(str)

segmentsArr = replicStr.split("[PAUSE]")

for i,segmentStr in enumerate(segmentsArr):
    
    paragsArr = segmentStr.split("[BREAK]")
    
    for j,paragStr in enumerate(paragsArr):
        
        wordsArr = re.split(r'\s(?:(?=(?:(?![\]\)}]).)*[\[{])|(?!.*[\]}]))', paragStr)
        wordsArr = re.split(" ", paragStr)
        
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

resStr = "[PAUSE]".join(segmentsArr)

print(resStr)
