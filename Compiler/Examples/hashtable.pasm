new		P0, .Hashtable
push	P0, "foo", 1
push	P0, "bar", "foo"

pop		I0, P0, "foo"
print	I0
print	"\n"

pop		S0, P0, "bar"
print	S0
print	"\n"

# =====================================

new		P1, .Hashtable
push	P1, "hashtable", P0
push	P1, "foo", 2.5

pop		P2, P1, "hashtable"

pop		I0, P2, "foo"
print	I0
print	"\n"

pop		S0, P2, "bar"
print	S0
print	"\n"

pop		N1, P1, "foo"
print	N1
print	"\n"