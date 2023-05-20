import typing
from collections import defaultdict
from typing import Any


# =================================================
# ======== Base Range
# =================================================

class Range:
    def __init__(self, iter: typing.Iterable):
        self.iter_obj = iter

    def __rshift__(self, rhs):
        return rhs(self)
    
    def __iter__(self):
        return iter(self.iter_obj)


# =================================================
# ======== Handlers
# =================================================
class Handler:
    def __call__(self, rng: Range) -> Range:
        raise NotImplementedError("Base class call")

class Where:
    def __init__(self, condition: typing.Callable):
        self.condition = condition
    
    def __call__(self, rng: Range) -> Range:
        def gen():
            for elem in rng:
                if self.condition(elem):
                    yield elem
        
        return Range(gen())

class Apply:
    def __init__(self, func: typing.Callable):
        self.func = func
    
    def __call__(self, rng: Range) -> Range:
        def gen():
            for elem in rng:
                yield self.func(elem)
        return Range(gen())

class Take:
    def __init__(self, n: int):
        self.n = n
    
    def __call__(self, rng: Range) -> Range:
        def gen():
            for k, elem in enumerate(rng):
                if k < self.n:
                    yield elem
                else:
                    break
        return Range(gen())

class GroupBy:
    def __init__(self, func: typing.Callable):
        self.func = func
    
    def __call__(self, rng: Range) -> Range:
        def gen():
            kv = defaultdict(list)
            for elem in list(rng):
                kv[self.func(elem)].append(elem)
            
            for item in kv.items():
                yield item

        return Range(gen())

class OrderBy:
    def __init__(self, func: typing.Callable):
        self.func = func

    def __call__(self, rng: Range) -> Range:
        def gen():
            for elem in sorted(list(rng), key=self.func):
                yield elem

        return Range(gen())

class ToList:
    def __call__(self, rng: Range) -> Range:
        return list(rng)
    
class Flatten:
    def __call__(self, rng: Range) -> Range:
        def gen():
            for sequence in rng:
                for elem in sequence:
                    yield elem
        return Range(gen())


# =================================================
# ======== Custom Ranges
# =================================================
    
class FibonacciRange(Range):
    def __init__(self):
        pass

    def __iter__(self):
        def gen():
            a, b = 1, 1
            while True:
                elem, a, b = a, b, a + b
                yield elem

        return gen()
    
class FileLineRange(Range):
    def __init__(self, filename: str):
        self.filename = filename

    def __iter__(self):
        def gen():
            with open(self.filename) as f:
                for line in f:
                    yield line

        return gen()


# =================================================
# ======== Tests
# =================================================
if __name__ == "__main__":
    print("======= Simple Range Example =========")
    print(
        Range(range(10))
        >> Where(lambda x : x % 2 == 0)
        >> Apply(lambda x : x ** 2)
        >> Take(5)
        >> ToList()
    )
    print()

    print("========= Fibonacci Example ==========")
    print(
        FibonacciRange()
        >> Where(lambda x : x % 3 == 0)
        >> Apply(lambda x : x ** 2 if x % 2 == 0 else x)
        >> Take(5)
        >> ToList()
    )
    print()

    for k in range(1, 4):
        print(f"======= Word Count Example {k} =========")
        print(
            FileLineRange(f".\linq\sample{k}.txt")
            >> Apply(lambda x : x.split())
            >> Flatten()
            >> GroupBy(lambda x : x)
            >> Apply(lambda x : (len(x[1]), x[0]))
            >> OrderBy(lambda x : (-x[0], x[1]))
            # >> Apply(lambda x : x[1])
            >> Take(10)
            >> ToList()
        )
        print()
