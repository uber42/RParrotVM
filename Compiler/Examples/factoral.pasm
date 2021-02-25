begin:
    branch main

factorial:
    pop I2

factorial_func:
    pop I0
    eq I0, 1, factorail_exit, factorial_next

factorial_next:
    mul I1, I0
    dec I0
    push I0
    branch factorial_func

factorail_exit:
    push I2
    ret

main:
    push I1
    set I1, 1

    push 8
    bsr factorial

    print "8! = "
    print I1
    print "\n"

    pop I1

    end