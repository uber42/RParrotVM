    set I0, 0
loop:
    print I0
    inc I0
    gt I0, 10, out, loop
out:
    print "end of program"
    end