import primes

# print(primes.factor_out("100"))
print(primes.factor_out(17))
assert primes.factor_out(100) == [2, 2, 5, 5], "Неверное разложение на просыте делители"
assert primes.factor_out(17) == 'Prime!', "Нужно вывести Prime! если число и так простое"


print('TESTS PASSED')
