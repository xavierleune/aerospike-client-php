function mytransform(rec, offset)
    rec['a'] = rec['a'] + offset
    rec['b'] = rec['a'] * offset
    rec['c'] = rec['a'] + rec['b']
    aerospike:update(rec)
end
