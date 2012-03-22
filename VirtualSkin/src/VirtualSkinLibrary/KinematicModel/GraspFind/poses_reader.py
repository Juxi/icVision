#!/usr/bin/python


def check_map(a_map):
    assert(len(a_map) > 0)
    n_vectors = 0
    
    for key, vectors in a_map.items():
        assert(len(vectors) > 0)
        if not n_vectors:
            n_vectors = len(vectors)
        else:
            assert(len(vectors) == n_vectors)
            vector_length = len(vectors[0])
            for vector in vectors:
                assert(len(vector) == vector_length)

def read_map(filename):
    data = open(filename).read()
    lines = [l.strip() for l in data.split("\n")]
    
    name = ""
    the_map = {}
    vectors = []
    for line in lines:
        if not len(line):
            continue
        try:
            numbers = [float(i) for i in line.split(" " )]
            vectors.append(numbers)
        except ValueError:
            try:
                if name:
                    the_map[name] = vectors
                name = line
                vectors = []
            except:
                raise
                
    the_map[name] = vectors

    check_map(the_map)
    return the_map

filename = "table_map_dense.save"
a_map = read_map(filename)
print a_map.keys()

