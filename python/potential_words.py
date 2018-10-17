"""
potential_words.py

>>> trie = EnglishTrie()
>>> trie.add_string("Hi")
>>> trie.add_string("He")
>>> trie.add_string("Hel")
>>> trie.add_string("Hei")
>>> trie.add_string("abc")
>>> trie.add_string("Hi there this is a long string")
>>> trie.add_string("Hi there this is a long stritwo")

>>> print list(trie.find_string("abc"))
[True]

>>> print list(trie.find_string("abd"))
['abc']

>>> print list(trie.find_string("Ho"))
['Hi', 'He']

>>> print list(trie.find_string("Hi there this is a long strigg"))
['Hi there this is a long stritw', 'Hi there this is a long strin']

>>> print list(trie.find_string("Hi there this is a long strign"))
['Hi there this is a long stritw', 'Hi there this is a long string']

>>> print list(trie.find_string("Hi there invalid"))
['Hi there this ']

>>> print list(trie.find_string("Hi so invalid"))
['Hi there th']

>>> print trie
<0 [<a [<b [<c []>]>]>,<H [<i [<  [<t [<h [<e [<r [<e [<  [<t [<h [<i [<s [<  [<i [<s [<  [<a [<  [<l [<o [<n [<g [<  [<s [<t [<r [<i [<t [<w [<o []>]>]>,<n [<g []>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>]>,<e [<i []>,<l []>]>]>]>
"""

class EnglishTrieNode(object):
	def __init__(self, char, parent):
		self.char = char
		self.parent = parent
		self.children = {}

	def add_child(self, child):
		if child.char not in self.children:
			self.children[child.char] = child
			return child
		return self.children[child.char]

	def __str__(self):
		return "<{0} [{1}]>".format(self.char, ','.join([str(child) for child in self.children.values()]))

class EnglishTrie(object):
	
	def __init__(self):
		self.root = EnglishTrieNode(0, None)

	def add_string(self, string):
		node = self.root
		for char in string:
			#print string, char, node
			current = EnglishTrieNode(char, node)
			node = node.add_child(current)

	def yield_options(self, node, string, idx, deep = None):
		for child in node.children.values():
			local_string = list(string)

			if len(local_string) > idx:
				local_string[idx] = child.char
			else:
				local_string.append(child.char)

			if (len(local_string) > (idx + 1) and local_string[idx + 1] not in child.children) \
				or (len(local_string) <= (idx)):
				for option in self.yield_options(child, local_string, idx + 1):
					yield option
			else:
				yield ''.join(local_string[:idx + 1])

	def find_string(self, string):
		node = self.root
		options = []
		idx = 0
		for char in string:
			if char in node.children:
				if options:
					print "found partial match"
				node = node.children[char]
			else:
				options = True
				for option in self.yield_options(node, string, idx):
					yield option
				return
			idx += 1

		if not options:
			yield True

	def __str__(self):
		return str(self.root)

if __name__ == "__main__":
	import doctest
	doctest.testmod()