int test(int x) {
	int y = x + 2
	int z = y * x + 2
	int ww
	ww = z * -2
	double dd = ww
	string s = "The result is %f"
	printf(s, dd)
	return (int)dd % 7
	
}

void test2(string t) {
	printf(t)
	double de = 2.4 + 3 - 4.2 + 5
}

boolean b = 4.5 > 3.7
boolean c = b && true
boolean dee = c || false
echob(!b)
echob(c)
echob(dee)
echoi(-1)
echod(-4.5)
echob(true)
echob(true != false)
echoi(test(5.5))

test2("Good bye")