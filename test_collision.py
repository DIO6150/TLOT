"""
from itertools import product

def compute_hash (s) :
	p = 102013
	m = 1e9 + 9
	hash_value = 0
	p_pow = 1

	for c in s:
		hash_value = (hash_value + (ord (c) - ord('a') + 1) * p_pow) % m
		p_pow = (p_pow * p) % m

	return hash_value

def generate_hashs(valid_chars, max_len):
    results = []
    for length in range(1, max_len + 1):
        for combo in product(valid_chars, repeat=length):
            results.append(compute_hash (''.join(combo)))
    return results

valid_chars = "azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN_-0123456789*+/"
strlenmax = 15

print ("generating hashes...")
hashs = generate_hashs (valid_chars, 15)
print ("hashes generated")

collision = []

print ("searching collisions...")
for h in hashs :
      if hashs.count (h) > 1 : collision.append (h)

print (collision)
"""