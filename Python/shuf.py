#!/usr/bin/python
import random, sys
from optparse import OptionParser

def main():
    version_msg = "%prog 2.0"
    usage_msg = """%prog [OPTION]...[FILE]

Output all lines from file(or stdin) in random order."""
    parser = OptionParser(version=version_msg, usage=usage_msg)
    parser.add_option("-r", "--repeat",
                      action="store_false",
                      dest="repeat",
                      default = True)
    parser.add_option("-i", "--input-range=LO-HI",
                      action="store",
                      dest="inputrange",
                      help="treach each number LO through HI as an input line")
    parser.add_option("-n", "--head-count",
                      help="output at most HEADCOUNT lines",
                      dest="headcount")
    options, args = parser.parse_args(sys.argv[1:])

    repeat=options.repeat

    isheadcount = False
    try:
        headcount=int(options.headcount)
        if headcount < 0:
           raise ValueError()
        if headcount == 0:
            return None
        isheadcount = True
    except:
        if options.headcount is not None:
            parser.error("invalid line count: '" + str(options.headcount) + "'")

    isinputrange = False
    try:
        inputrange=str(options.inputrange)
        Range=inputrange.split("-")
        if len(Range)!=2:
            raise ValueError()
        LowLimit=int(Range[0])
        HighLimit=int(Range[1])
        
        if LowLimit>HighLimit:
            raise ValueError()
        isinputrange = True
    except:
        if options.inputrange is not None:
            parser.error("invalid input range: " + str(options.inputrange))

    if len(args) == 1:
        if args[0] == "-":
            lines = sys.stdin.readlines()
        else:
            lines = open(args[0]).readlines()
    elif len(args)==0:
        if not isinputrange:
            lines = sys.stdin.readlines()
        else:
            lines = []
            for x in range(LowLimit, HighLimit+1):
                lines.append(str(x)+"\n")
    else:
        parser.error("extra operand '" + str(args[1]) + "'")

    def output1line(n):
        index=0
        if isheadcount:
            index=len(n)-headcount
        while len(n)>index:
            try:
                a = random.randint(0, len(n)-1)
            except:
                return None
            for x in range(len(n)):
                if x == a:
                    sys.stdout.write(n[a])
                    if repeat:
                        n.remove(n[a])
                    elif isheadcount:
                        index = index + 1
                    
    output1line(lines)
    
if __name__ == "__main__":
    main()
