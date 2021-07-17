int a = 2

int test(int x) {
	int y = x + a
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
	//gets()
	double de = 2.4 + 3 - 4.2 + 5
}

boolean b = ((4 + 3) > (2.3 - 4.6)) && (3 > 4)
boolean c = b && true
boolean dee = c || false
if (b) {
	printf("B is true")
	int x = 100
	echoi(x)
} else {
	printf("B is false")
	if (c) {
		printf("C is true")
	} else {
		printf("C is false")
	}
}
echob(!b)
echob(c)
echob(dee)
echoi(-1)
echod(-4.5)
echob(true)
echob(true != false)
echoi(test(5.5))

test2("Good bye")