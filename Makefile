primes: primes.c
	gcc -std=c99 -o p primes.c -lm

clear:
	rm p
